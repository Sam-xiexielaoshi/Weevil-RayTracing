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

void Renderer::Render(const Camera& camera)
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
			
			glm::vec4 color = TraceRay(ray);
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

glm::vec4 Renderer::TraceRay(const Ray& ray)
{
	float radius = 0.5f;
	//rayDirection = glm::normalize(rayDirection);

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

	//glm::vec3 oc = rayOrigin - sphereCenter; //oc is the vector from the ray origin to the sphere center

	float a = glm::dot(ray.Direction, ray.Direction); //ray origin
	float b = 2.0f * glm::dot(ray.Origin, ray.Direction); //ray direction
	//float b = 2.0f * glm::dot(oc, rayDirection); //ray direction
	//float c = glm::dot(oc, oc) - radius * radius;
	float c = glm::dot(ray.Origin, ray.Origin) - radius * radius; //since we are assuming the sphere center is at the origin we can simplify the eq to this



	//quad eq formual discriminant = b^2 - 4ac
	float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0.0f)
		return glm::vec4(0,0,0,1);

	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
	if(t0<0.0f)
		return glm::vec4(0,0,0,1);

	glm::vec3 hitPoint = ray.Origin + closestT * ray.Direction;//comp hit poiint
	glm::vec3 normal = glm::normalize(hitPoint);//comp normal

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
	float intensity = glm::max(glm::dot(normal, -lightDir), 0.0f); // this is N * L classic lambertian diffuse lighting equation
	//float ambient = 0.1f;
	//float lighting = glm::min(ambient + intensity, 1.0f);
	glm::vec3 sphereColor(1.0f, 0.0f, 1.0f);
	sphereColor = normal * intensity;
	return glm::vec4(sphereColor, 1.0f); //abgr format

	//glm::vec3 viewDir = glm::normalize(rayOrigin - hitPoint);
	//glm::vec3 reflectDir = glm::reflect(lightDir, normal);
	//float specular = pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), 8.0f); // specular highlight with shininess of 8
	//glm::vec3 color = sphereColor * lighting + glm::vec3(specular); // apply lighting to the sphere color
	//color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f)); // clamp color to the range of 0 to 1

	//uint8_t rCol = (uint8_t)(color.r * 255.0f);
	//uint8_t gCol = (uint8_t)(color.g * 255.0f);
	//uint8_t bCol = (uint8_t)(color.b * 255.0f);
	//return glm::vec4(rCol / 255.0f, gCol / 255.0f, bCol / 255.0f, 1.0f); //abgr format
	//return 0xff000000 | (g << 8) | r;

	
}
