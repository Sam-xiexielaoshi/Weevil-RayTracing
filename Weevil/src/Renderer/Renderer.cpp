#include "Renderer.h"
#include "Walnut/Random.h"
#include <execution>

Renderer::Renderer()
{
	if (!m_Environment.LoadHDR("assets/HDR/studio_small_08_4k.hdr"))
	{
		std::cout << "Failed to load HDR image" << std::endl;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	//major flaw is that i still hve to generate a new image every time i resize the
	// window which is not good for performance but i will fix that later on by just 
	// resizing the existing image instead of creating a new one every time

	if (m_FinalImage)
	{
		//no resize needed if the image is not created yet
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height) return;

		m_FinalImage->Resize(width, height);
	}
	else 
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
	
	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	delete[] m_HDRImage;
	m_HDRImage = new glm::vec4[width * height];

	delete[] m_BloomImage;
	m_BloomImage = new glm::vec4[width * height];

	delete[] m_BlurImage;
	m_BlurImage = new glm::vec4[width * height];

	m_ImageHorizontalIterator.resize(width);
	m_ImageVerticalIterator.resize(height);

	for(uint32_t i =0; i < width; i++)
		m_ImageHorizontalIterator[i] = i;

	for(uint32_t i =0; i < height; i++)
		m_ImageVerticalIterator[i] = i;

	GenerateGaussianKernel();
}

glm::vec4 Renderer::Integrate(uint32_t x, uint32_t y)
{
	PathState pathState;
	pathState.CurrentRay.Origin = m_ActiveCamera->GetPosition();
	pathState.CurrentRay.Direction = m_ActiveCamera->GetRayDirections()[y * m_FinalImage->GetWidth() + x];

	const int bounces = m_Settings.MaxBounces;//now instead of using a fixed bounce amt to terminate we will implement russian roulette to terminate the ray if it has a low probability of contributing to the final image
	for (int i = 0; i < bounces; i++)
	{
		HitPayload payload = TraceRay(pathState.CurrentRay);
		if (payload.HitDistance < 0.0f)
		{
			AddSkyLight(pathState.AccumulatedRadiance, pathState.PathThroughput, pathState.CurrentRay);
			break;
		}

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

		if (pathState.CurrentRay.InsideMedium && payload.HitDistance > 0.0f)
		{
			glm::vec3 transmittance = glm::exp(-pathState.CurrentRay.AbsorptionCoefficient * payload.HitDistance);
			pathState.PathThroughput *= transmittance;
		}

		AddEmission(pathState.AccumulatedRadiance, pathState.PathThroughput, material);
		if (material.Type == MaterialType::Diffuse)
		{
			pathState.AccumulatedRadiance += pathState.PathThroughput * EstimateDirectLighting(payload, material);
		}

		BSDFSample sample = SampleBSDF(pathState.CurrentRay, payload, material);

		pathState.PathThroughput *= sample.Weight;
		pathState.CurrentRay.Direction = sample.Direction;
		OffsetRayOrigin(pathState.CurrentRay, payload);
		if (!RussianRoulette(pathState.PathThroughput, i))
			break;
	}
	
	return glm::vec4(pathState.AccumulatedRadiance, 1.0f); //abgr format
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	if (m_FrameIndex == 1)
	{
		m_BVH.Build(scene);
	}

	BuildLightList();

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

	RayTrace();	
	Accumulate();
	Bloom();

	ApplyExposure();
	ApplyToneMapping();
	ApplyGammaCorrection();

	ConvertToRGBA();
	Present();

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

void Renderer::RayTrace()
{
	ForEachPixel([this](uint32_t x, uint32_t y, uint32_t index)
	{
		m_HDRImage[index] = Integrate(x, y);
	});
}

void Renderer::Accumulate()
{
	ForEachPixel([this](uint32_t, uint32_t, uint32_t index)
	{
		m_AccumulationData[index] += m_HDRImage[index];

		m_HDRImage[index] =
			m_AccumulationData[index] / (float)m_FrameIndex;
	});
}

void Renderer::AddSkyLight(glm::vec3& light, const glm::vec3& throughput, const Ray& ray)
{
	glm::vec3 skyColor = m_Environment.Sample(ray.Direction); // Sample the environment map in the upward direction
	light += skyColor * throughput;
}

void Renderer::AddEmission(glm::vec3& light, glm::vec3& throughput, const Material& material)
{
	light += throughput * material.GetEmission();
}

void Renderer::BuildLightList()
{
	m_Lights.clear();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
		if(material.EmissionPower > 0.0f)
			m_Lights.push_back({ static_cast<int>(i), sphere.MaterialIndex });
	}
}

glm::vec3 Renderer::EstimateDirectLighting(const HitPayload& payload, const Material& material)
{
	if(m_Lights.empty())
		return glm::vec3(0.0f);
	int lightIndex = Walnut::Random::UInt() % m_Lights.size();
	const Light& light = m_Lights[lightIndex];
	const Sphere& sphere = m_ActiveScene->Spheres[light.SphereIndex];
	const Material& lightMaterial = m_ActiveScene->Materials[light.MaterialIndex];
	
	//uniform sphere ssampleing
	float u = Walnut::Random::Float();
	float v = Walnut::Random::Float();

	float theta = 2.0f * glm::pi<float>() * u;
	float phi = glm::acos(1.0f - 2.0f * v);

	glm::vec3 samplePoint;
	samplePoint.x = sin(phi) * cos(theta);
	samplePoint.y = cos(phi);
	samplePoint.z = sin(phi) * sin(theta);

	samplePoint = sphere.Position + samplePoint * sphere.Radius;

	//direction from shafding point to sasmpled point
	glm::vec3 toLight = samplePoint - payload.WorldPosition;
	glm::vec3 lightNormal = glm::normalize(samplePoint - sphere.Position);


	float distance2 = glm::dot(toLight, toLight);
	float distance = glm::sqrt(distance2);
	glm::vec3 lightDirection = toLight/distance;

	float lightCos = glm::max(glm::dot(lightNormal, -lightDirection), 0.0f);
	
	if(lightCos <= 0.0f)
		return glm::vec3(0.0f);
	
	constexpr float kRayBias = 1e-4f;
	Ray shadowRay;
	shadowRay.Origin = payload.WorldPosition + payload.WorldNormal * kRayBias;
	shadowRay.Direction = lightDirection;
	if (IsOccluded(shadowRay, distance, light.SphereIndex))
		return glm::vec3(0.0f);
	
	float NdotL = glm::max(glm::dot(payload.WorldNormal, lightDirection), 0.0f);

	if(NdotL <= 0.0f)
		return glm::vec3(0.0f);

	float pdf = 1.0f / (4.0f * glm::pi<float>() * sphere.Radius * sphere.Radius);
	glm::vec3 brdf = material.Albedo / glm::pi<float>();
	glm::vec3 radiance = lightMaterial.GetEmission() * brdf * NdotL * lightCos / distance2;
	radiance /= pdf;
	radiance *= static_cast<float>(m_Lights.size());
	return radiance;
}

