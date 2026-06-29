#pragma once
#include "Walnut/Image.h"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include <memory>
#include <glm/glm.hpp>
#include <execution>
#include <algorithm>

class Renderer
{
public:
	struct Settings
	{
		bool Accumate = true;
		bool SlowRandom = true;
	};
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResertFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }

private:
	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y);//raygen

	template<typename T>
	void ForEachPixel(T&& t);

	void RayTrace();
	void Accumulate();
	void Bloom();
	void ToneMap();
	void GammaCorrection();
	void ConvertToRGBA();
	void Present();

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance,int objectIndex);
	HitPayload Miss(const Ray& ray);

private:
	std::shared_ptr<Walnut::Image>m_FinalImage;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIterator, m_ImageVerticalIterator;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	glm::vec4* m_HDRImage = nullptr;

	uint32_t m_FrameIndex = 1;


};

template<typename T>
void Renderer::ForEachPixel(T&& t)
{
#define MT 1

#if MT

		const uint32_t width = m_FinalImage->GetWidth();

		std::for_each(std::execution::par,
			m_ImageVerticalIterator.begin(),
			m_ImageVerticalIterator.end(),
			[this, &t, width](uint32_t y)
			{
				std::for_each(std::execution::par,
					m_ImageHorizontalIterator.begin(),
					m_ImageHorizontalIterator.end(),
					[&, y](uint32_t x)
					{
						uint32_t index = y * width + x;
						t(x, y, index);
					});
			});

#else

		const uint32_t width = m_FinalImage->GetWidth();
		const uint32_t height = m_FinalImage->GetHeight();

		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				uint32_t index = y * width + x;
				t(x, y, index);
			}
		}

#endif
}