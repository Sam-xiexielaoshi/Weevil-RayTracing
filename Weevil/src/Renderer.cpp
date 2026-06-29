#include "Renderer.h"
#include "Walnut/Random.h"
#include <execution>

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& light)
	{
		uint8_t r = static_cast<uint8_t>(light.r * 255.0f);
		uint8_t g = static_cast<uint8_t>(light.g * 255.0f);
		uint8_t b = static_cast<uint8_t>(light.b * 255.0f);
		uint8_t a = static_cast<uint8_t>(light.a * 255.0f);
		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
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
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	//Ray ray;
	//ray.Origin = m_ActiveCamera->GetPosition();
	//ray.Direction =	m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
	//glm::vec3 light(0.0f);
	////float throughput = 1.0f;
	//glm::vec3 throughtput(1.0f);
	//int bounces = 5; //number of bounces for the ray
	//for(int i = 0; i < bounces; i++)
	//{
	//	Renderer::HitPayload payload = TraceRay(ray);
	//	if (payload.HitDistance < 0.0f)
	//	{
	//		glm::vec3 skyColor(0.6f, 0.7f, 0.9f); //light blue light for the sky
	//		light += skyColor * throughtput;
	//		break;
	//	}
	//	const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
	//	const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
	//	//direct lightinh
	//	glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
	//	float NdotL = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // this is N * L classic lambertian diffuse lighting equation
	//
	//	glm::vec3 sphereColor = material.Albedo;
	//	sphereColor *= intensity;
	//	glm::vec3 diffuse = material.Albedo * NdotL * (1.0f - material.Metallic); //diffuse component
	//	light += diffuse * throughtput; //u can put spherelight here but i will use the diffuse component instead to make it more realistic
	//	throughtput *= 0.5f; //reduce the throughput for the next bounce basically ignores the material
	//	//update throughput
	//	glm::vec3 attenuation;
	//	if (material.Metallic > 0.5f)
	//	{
	//		attenuation = material.Albedo; //if the material is metallic use the albedo light as the attenuation
	//	}
	//	else
	//	{
	//		attenuation = glm::vec3(1.0f);
	//		//if the material is not metallic use white light as the attenuation
	//	}
	//	throughtput *= attenuation; //reduce the throughput for the next bounce based on the attenuation of the material
	//	throughtput *= glm::mix(0.95f, 0.5f, material.Roughness); //reduce the throughput for the next bounce based on the roughness of the material
	//	ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f; //offset the origin to avoid self intersection
	//	glm::vec3 random = material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f); //add some randomness to the reflection based on the roughness of the material
	//	ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f)); //reflect the ray direction based on the normal
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 light(0.0f);
	glm::vec3 throughput(1.0f);

	int bounces = 5; //number of bounces for the ray
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f); //light blue light for the sky
			//light += skyColor * throughput;
			break;
		}

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

		throughput *= material.Albedo; //reduce the throughput for the next bounce
		light += material.GetEmission();

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f; //offset the origin to avoid self intersection
		//ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f)); //reflect the ray direction based on the normal
		ray.Direction = glm::normalize(payload.WorldNormal + Walnut::Random::InUnitSphere()); //random direction for the next bounce
	}

	return glm::vec4(light, 1.0f); //abgr format
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

	RayTrace();	
	Accumulate();
	Bloom();
	ToneMap();
	GammaCorrection();
	ConvertToRGBA();
	Present();

	if (m_Settings.Accumate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

void Renderer::RayTrace()
{
	ForEachPixel([this](uint32_t x, uint32_t y, uint32_t index)
	{
		m_HDRImage[index] = PerPixel(x, y);
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

void Renderer::Bloom()
{
	ExtractBrightPass();
	BlurHorizontal();
	BlurVertical();
	CombineBloom();
}

void Renderer::ToneMap()
{
}

void Renderer::GammaCorrection()
{
}

void Renderer::ConvertToRGBA()
{
	ForEachPixel([this](uint32_t, uint32_t, uint32_t index)
	{
		glm::vec4 color = glm::clamp(m_HDRImage[index], glm::vec4(0.0f), glm::vec4(1.0f));
		m_ImageData[index] = Utils::ConvertToRGBA(color);
	});
}

void Renderer::Present()
{
	m_FinalImage->SetData(m_ImageData);
}

void Renderer::ExtractBrightPass()
{
	ForEachPixel([this](uint32_t, uint32_t, uint32_t index)
	{
		glm::vec4 color = m_HDRImage[index];

		float brightness = glm::dot(glm::vec3(color), glm::vec3(0.2126f, 0.7152f, 0.0722f)); //luminance formula
		if (brightness > m_Settings.BloomThreshold)
			m_BloomImage[index] = color;
		else
			m_BloomImage[index] = glm::vec4(0.0f);
	});
}

void Renderer::BlurHorizontal()
{		
	const int bloomRadius = m_Settings.BloomRadius;
	const uint32_t width = m_FinalImage->GetWidth();
	ForEachPixel([this, bloomRadius, width](uint32_t x, uint32_t y, uint32_t index)
	{
		glm::vec4 blurredColor(0.0f);
		int count = 0;
		for (int offset = -bloomRadius; offset <= bloomRadius; offset++)
		{
			int sampleX = (int)x + offset;
			if (sampleX < 0 || sampleX >= (int)width)
				continue;
			uint32_t sampleIndex = y * width + sampleX;

			blurredColor += m_BloomImage[sampleIndex];
			count++;
		}
		m_BlurImage[index] = blurredColor / (float)count;
	});
	
}

void Renderer::BlurVertical()
{
	const int bloomRadius = m_Settings.BloomRadius;
	const uint32_t width = m_FinalImage->GetWidth();
	const uint32_t height = m_FinalImage->GetHeight();
	ForEachPixel([this, bloomRadius, width, height](uint32_t x, uint32_t y, uint32_t index)
	{
		glm::vec4 blurredColor(0.0f);
		int count = 0;

		for (int offset = -bloomRadius; offset <= bloomRadius; offset++)
		{
			int sampleY = (int)y + offset;
			if (sampleY < 0 || sampleY >= (int)height)
				continue;
			uint32_t sampleIndex = sampleY * width + x;
			blurredColor += m_BlurImage[sampleIndex];
			count++;
		}
		m_BloomImage[index] = blurredColor / (float)count;
	});
}

void Renderer::CombineBloom()
{
	const float bloomStrength = m_Settings.BloomStrength;
	ForEachPixel([this, bloomStrength](uint32_t, uint32_t, uint32_t index)
	{
		glm::vec4 bloom = m_BloomImage[index] * bloomStrength;
		m_HDRImage[index] += bloom;
	});
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	//ray eq -> p(t) = rayOrigin + t * rayDirection
	//sphere eq -> |spherecenter - sphereradius|^2 = r^2
	//sub the ray eq into the sphere eq and we get -> |(rayOrigin + t * rayDirection) - sphereCenter|^2 = r^2
	//assuming , spherecenter is at the origin we can simplify the eq to -> |(rayOrigin + t * rayDirection)|^2 = r^2
	//expanding the eq we get -> |rayOrigin|^2 + 2t(rayOrigin . rayDirection) + t^2 |rayDirection|^2 = r^2
	//using the formula
	//(bx^2 + by^2)t^2 + (2axbx + 2ayby)t + (ax^2 + ay^2 - r^2) = 0
	// a = ray origin 
	// b = ray direction 
	// r = radius
	// t = hit radius

	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		glm::vec3 origin = ray.Origin - sphere.Position; //origin is the vector from the ray origin to the sphere center

		float a = glm::dot(ray.Direction, ray.Direction); //ray origin
		float b = 2.0f * glm::dot(origin, ray.Direction); //ray direction
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius; //since we are assuming the sphere center is at the origin we can simplify the eq to this
		//quad eq formual discriminant = b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f)
			continue;

		//float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if(closestT < hitDistance && closestT > 0.0f)
		{
			hitDistance = closestT;
			closestSphere = static_cast<int>(i);
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = origin + hitDistance * ray.Direction;//comp hit poiint
	payload.WorldNormal = glm::normalize(payload.WorldPosition);//comp normal

	payload.WorldPosition += closestSphere.Position; //translate the hit point back to world space

	return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}