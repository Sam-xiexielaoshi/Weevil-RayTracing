#include "../Renderer.h"
#include "../../Utils/RendererUtils.h"

void Renderer::Bloom()
{
	ExtractBrightPass();
	BlurHorizontal();
	BlurVertical();
	CombineBloom();
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
	if (m_Settings.BloomFilter == BloomFilter::Box)
	{
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
		return;
	}
	else
	{
		ForEachPixel([this, bloomRadius, width](uint32_t x, uint32_t y, uint32_t index)
			{
				glm::vec4 blurredColor(0.0f);
				float totalWeight = 0.0f;
				for (int offset = -bloomRadius; offset <= bloomRadius; offset++)
				{
					int sampleX = (int)x + offset;
					if (sampleX < 0 || sampleX >= (int)width)
						continue;
					uint32_t sampleIndex = y * width + sampleX;
					float weight = m_GaussianKernel[offset + bloomRadius];
					blurredColor += m_BloomImage[sampleIndex] * weight;
					totalWeight += weight;
				}
				m_BlurImage[index] = blurredColor / totalWeight;
			});
	}
}

void Renderer::BlurVertical()
{
	const int bloomRadius = m_Settings.BloomRadius;
	const uint32_t width = m_FinalImage->GetWidth();
	const uint32_t height = m_FinalImage->GetHeight();
	if (m_Settings.BloomFilter == BloomFilter::Box)
	{
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
		return;
	}
	else
	{
		ForEachPixel([this, bloomRadius, width, height](uint32_t x, uint32_t y, uint32_t index)
			{
				glm::vec4 blurredColor(0.0f);
				float totalWeight = 0.0f;
				for (int offset = -bloomRadius; offset <= bloomRadius; offset++)
				{
					int sampleY = (int)y + offset;
					if (sampleY < 0 || sampleY >= (int)height)
						continue;
					uint32_t sampleIndex = sampleY * width + x;
					float weight = m_GaussianKernel[offset + bloomRadius];
					blurredColor += m_BlurImage[sampleIndex] * weight;
					totalWeight += weight;
				}
				m_BloomImage[index] = blurredColor / totalWeight;
			});
	}
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
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void Renderer::ApplyExposure()
{
	ForEachPixel([this](uint32_t, uint32_t, uint32_t index)
		{
			m_HDRImage[index] *= m_Settings.Exposure;
		});
}

void Renderer::ApplyToneMapping()
{
	ForEachPixel([this](uint32_t, uint32_t, uint32_t index)
		{
			glm::vec3 color = glm::vec3(m_HDRImage[index]);
			switch (m_Settings.ToneMapping)
			{
			case ToneMapper::None:
				break;
			case ToneMapper::Reinhard:
				color = ReinhardToneMap(color);
				break;
			case ToneMapper::ACES:
				color = ACESToneMap(color);
				break;
			case ToneMapper::Hable:
				color = HableToneMap(color);
				break;
			}
			m_HDRImage[index] = glm::vec4(color, m_HDRImage[index].a);
		});
}

void Renderer::ApplyGammaCorrection()
{
	const float inverseGamma = 1.0f / m_Settings.Gamma;
	ForEachPixel([this, inverseGamma](uint32_t, uint32_t, uint32_t index)
		{
			glm::vec4& color = m_HDRImage[index];
			color.r = std::pow(color.r, inverseGamma);
			color.g = std::pow(color.g, inverseGamma);
			color.b = std::pow(color.b, inverseGamma);
		});
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

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void Renderer::GenerateGaussianKernel()
{
	m_GaussianKernel.clear();
	const int radius = m_Settings.BloomRadius;
	const float sigma = std::max(0.5f, radius * 0.5f);
	float totalWeight = 0.0f;
	for (int i = -radius; i <= radius; i++)
	{
		float weight = std::exp(-(i * i) / (2.0f * sigma * sigma));
		m_GaussianKernel.push_back(weight);
		totalWeight += weight;
	}
	// Normalize the kernel
	for (float& weight : m_GaussianKernel)
		weight /= totalWeight;
}

glm::vec3 Renderer::ReinhardToneMap(const glm::vec3& color)
{
	return color / (color + glm::vec3(1.0f));
}

glm::vec3 Renderer::ACESToneMap(const glm::vec3& color)
{
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;

	glm::vec3 result = (color * (a * color + b)) / (color * (c * color + d) + e);
	return glm::clamp(result, glm::vec3(0.0f), glm::vec3(1.0f));
}

glm::vec3 Renderer::HableToneMap(const glm::vec3& color)
{
	//needed to be done in future
	return ACESToneMap(color);
}