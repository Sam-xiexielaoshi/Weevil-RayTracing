#include "RendererUtils.h"

namespace Utils
{
	uint32_t ConvertToRGBA(const glm::vec4& light)
	{
		uint8_t r = static_cast<uint8_t>(light.r * 255.0f);
		uint8_t g = static_cast<uint8_t>(light.g * 255.0f);
		uint8_t b = static_cast<uint8_t>(light.b * 255.0f);
		uint8_t a = static_cast<uint8_t>(light.a * 255.0f);
		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

}