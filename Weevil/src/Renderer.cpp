#include "Renderer.h"
#include "Walnut/Random.h"


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

void Renderer::Render()
{
	for (uint32_t j = 0; j < m_FinalImage->GetHeight(); j++)
	{
		// Rendering code goes here rendereing every pixel 
		for (uint32_t i = 0; i < m_FinalImage->GetWidth(); i++)
		{
			glm::vec2 coord = { (float)i / (float)m_FinalImage->GetWidth(), (float)j / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; //convert -1 to 1 range btw not the final sol
			m_ImageData[j * m_FinalImage->GetWidth() + i] = PerPixel(coord);

			//m_ImageData[i] = 0xFFFF00FF; //abgr format first 2 are alpha channel, next two are blue channel then next two are green channel and finally last 2 remaining are red channel;
			//m_ImageData[i] = Walnut::Random::UInt();
			//m_ImageData[i] |= 0xFF000000; // Set alpha channel to 255
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;
	//raqdirection = glm::normalize(rayDirection);

	//using the formula
	//(bx^2 + by^2)t^2 + (2axbx + 2ayby)t + (ax^2 + ay^2 - r^2) = 0
	// a = ray origin 
	// b = ray direction 
	// r = radius
	// t = hit radius

	float a = glm::dot(rayDirection, rayDirection); //ray origin
	float b = 2.0f * glm::dot(rayOrigin, rayDirection); //ray direction
	float c = glm::dot(rayOrigin, rayOrigin) -radius * radius;

	//quad eq formual discriminant = b^2 - 4ac
	float discriminant = b * b - 4.0f * a * c;

	if (discriminant >= 0.0f)
		return 0xffff00ff;

	return 0xff000000;
	//return 0xff000000 | (g << 8) | r;
}
