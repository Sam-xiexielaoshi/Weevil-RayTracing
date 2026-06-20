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

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	const glm::vec3& rayOrigin = camera.GetPosition(); 

	Ray ray;
	ray.Origin = camera.GetPosition();

	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		// Rendering code goes here rendereing every pixel 
		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			ray.Direction = camera.GetRayDirections()[i + j * m_FinalImage->GetWidth()];
			
			glm::vec4 color = TraceRay(scene, ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[j * m_FinalImage->GetWidth() + i] = Utils::ConvertToRGBA(color);

			//m_ImageData[i] = 0xFFFF00FF; //abgr format first 2 are alpha channel, next two are blue channel then next two are green channel and finally last 2 remaining are red channel;
			//m_ImageData[i] = Walnut::Random::UInt();
			//m_ImageData[i] |= 0xFF000000; // Set alpha channel to 255
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

//Generate ray
//
//Sphere intersection
//
//Compute hit point
//
//Compute normal
//
//Lambert lighting
//
//Ambient lighting
//
//Color packing

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
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
	if(scene.Spheres.size()==0)
		return glm::vec4(0, 0, 0, 1);

	const Sphere* closestSphere = nullptr;
	float hitDistance = std::numeric_limits<float>::max();

	for (const Sphere& sphere : scene.Spheres)
	{
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
			closestSphere = &sphere;
		}
	}

	if (closestSphere == nullptr)
		return glm::vec4(0, 0, 0, 1);

	glm::vec3 origin = ray.Origin - closestSphere->Position; 

	glm::vec3 hitPoint = origin + hitDistance * ray.Direction;//comp hit poiint

	glm::vec3 normal = glm::normalize(hitPoint);//comp normal

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
	float intensity = glm::max(glm::dot(normal, -lightDir), 0.0f); // this is N * L classic lambertian diffuse lighting equation

	glm::vec3 sphereColor = closestSphere->Albedo;
	sphereColor *= intensity;
	return glm::vec4(sphereColor, 1.0f); //abgr format
}
