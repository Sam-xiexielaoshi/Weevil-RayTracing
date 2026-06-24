#include "Renderer.h"
#include "Walnut/Random.h"

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
		uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
		uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
		uint8_t a = static_cast<uint8_t>(color.a * 255.0f);
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
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction =	m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 color(0.0f);

	float multiplier = 1.0f;

	int bounces = 2; //number of bounces for the ray
	for(int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f); //light blue color for the sky
			color += skyColor * multiplier;
			break;
		}

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
		float intensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // this is N * L classic lambertian diffuse lighting equation

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		glm::vec3 sphereColor = sphere.Albedo;
		sphereColor *= intensity;
		color += sphereColor * multiplier;
		multiplier *= 0.7f; //reduce the multiplier for the next bounce

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f; //offset the origin to avoid self intersection
		ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal); //reflect the ray direction based on the normal

	}

	return glm::vec4(color, 1.0f); //abgr format
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		// Rendering code goes here rendereing every pixel 
		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			PerPixel(i, j);
			
			glm::vec4 color = PerPixel(i,j);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[j * m_FinalImage->GetWidth() + i] = Utils::ConvertToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_ImageData);
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