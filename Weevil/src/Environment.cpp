#include "Environment.h"
#include <stb_image.h>
#include <iostream>

bool Environment::LoadHDR(const std::string& filepath)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	float* data = stbi_loadf(filepath.c_str(), &width, &height, &channels, 3);
	if (!data)
		return false;

	m_Width = width;
	m_Height = height;
	m_Pixels.resize(width * height);
	for (uint32_t y = 0; y < height; ++y)
	{
		for(uint32_t x = 0; x < width; ++x)
		{
			uint32_t index = y * width + x;
			m_Pixels[index] = {data[index * 3], data[index * 3 + 1], data[index * 3 + 2]};
		}
	}
	float maxValue = 0.0f;

	for (const auto& pixel : m_Pixels)
	{
		maxValue = std::max(maxValue, pixel.r);
		maxValue = std::max(maxValue, pixel.g);
		maxValue = std::max(maxValue, pixel.b);
	}

	std::cout << "HDR Max Value = " << maxValue << std::endl;
	stbi_image_free(data);
	return true;
}

glm::vec3 Environment::Sample(const glm::vec3& direction) const
{
	if (m_Pixels.empty())
		return glm::vec3(0.6f, 0.7f, 0.9f);

	glm::vec3 dir = glm::normalize(direction);

	// Rotate around Y-axis
	float angle = glm::radians(m_Rotation);
	float c = cos(angle);
	float s = sin(angle);

	glm::vec3 rotated;
	rotated.x = dir.x * c - dir.z * s;
	rotated.y = dir.y;
	rotated.z = dir.x * s + dir.z * c;

	// Standard lat-long mapping
	float u = 0.5f + atan2(rotated.z, rotated.x) / (2.0f * glm::pi<float>());
	float v = 0.5f - asin(glm::clamp(rotated.y, -1.0f, 1.0f)) / glm::pi<float>();

	uint32_t x = static_cast<uint32_t>(u * (m_Width - 1));
	uint32_t y = static_cast<uint32_t>((1.0f - v) * (m_Height - 1));

	x = glm::clamp(x, 0u, m_Width - 1);
	y = glm::clamp(y, 0u, m_Height - 1);
	glm::vec3 color = m_Pixels[y * m_Width + x];

	static bool printed = false;

	if (!printed)
	{
		std::cout << "Sample = "
			<< color.r << ", "
			<< color.g << ", "
			<< color.b << std::endl;

		printed = true;
	}

	return color * m_Exposure;
}