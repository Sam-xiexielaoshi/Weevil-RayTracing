#pragma once
#include "Walnut/Image.h"

#include "BVH/BVH.h"
#include "BSDF/BSDFSample.h"
#include "../Core/Camera.h"
#include "../Scene/Ray.h"
#include "../Scene/Scene.h"
#include "../Scene/Environment.h"
#include "Integrator/HitPayload.h"

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>
#include <execution>
#include <algorithm>
#include <vector>
#include <iostream>

class Renderer
{
public:
	enum class BloomFilter
	{
		Box,
		Gaussian
	};
	enum class ToneMapper
	{
		None = 0,
		Reinhard,
		ACES,  
		Hable
	};
	struct Settings
	{
		bool Accumulate = true;
		bool SlowRandom = true;
		bool UseBVH = true;
		BloomFilter BloomFilter = BloomFilter::Gaussian;
		float BloomThreshold = 1.0f;
		//bool ShowBloomBuffer = true;
		int BloomRadius = 10;
		float BloomStrength = 1.0f;
		ToneMapper ToneMapping = ToneMapper::ACES;
		float Exposure = 1.0f;
		float Gamma = 2.2f;
		bool EnableRussianRoulette = true;
		int MaxBounces = 5;
	};
public:
	Renderer();

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }

	void GenerateGaussianKernel();

	uint32_t GetFinalIndex() const { return m_FrameIndex; }

	Environment& GetEnvironment() { return m_Environment; }

private:

	struct PathState
	{
		Ray CurrentRay;
		glm::vec3 PathThroughput = glm::vec3(1.0f);
		glm::vec3 AccumulatedRadiance = glm::vec3(0.0f);
	};

	struct Light
	{
		int SphereIndex;
		int MaterialIndex;
	};
	glm::vec4 Integrate(uint32_t x, uint32_t y);//raygen

	template<typename T>
	void ForEachPixel(T&& t);

	void RayTrace();
	void Accumulate();
	void Bloom();

	void ApplyExposure();
	void ApplyToneMapping();
	void ApplyGammaCorrection();

	void ConvertToRGBA();
	void Present();

	void ExtractBrightPass();
	void BlurHorizontal();
	void BlurVertical();
	void CombineBloom();

	glm::vec3 ReinhardToneMap(const glm::vec3& color);
	glm::vec3 ACESToneMap(const glm::vec3& color);
	glm::vec3 HableToneMap(const glm::vec3& color);

	BSDFSample SampleDiffuse(const Ray& ray, const HitPayload& payload, const Material& material);
	BSDFSample SampleMetal(const Ray& ray, const HitPayload& payload, const Material& material);
	BSDFSample SampleDielectric(Ray& ray, const HitPayload& payload, const Material& material);
	BSDFSample SampleMirror(const Ray& ray, const HitPayload& payload, const Material& material);

	BSDFSample SampleBSDF(Ray& ray, const HitPayload& payload, const Material& material);

	bool RussianRoulette(glm::vec3& throughput, int bounce);

	void AddSkyLight(glm::vec3& light, const glm::vec3& throughput, const Ray& ray);

	void AddEmission(glm::vec3& light, glm::vec3& throughput, const Material& material);

	void OffsetRayOrigin(Ray& ray,const HitPayload& payload);

	HitPayload TraceRayBruteForce(const Ray& ray);
	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance,int objectIndex);
	HitPayload Miss(const Ray& ray);

	void BuildLightList();

	glm::vec3 EstimateDirectLighting(const HitPayload& payload, const Material& material);
	bool IsOccluded(const Ray& shadowRay, float maxDistance, int ignoredSphere);

private:
	std::shared_ptr<Walnut::Image>m_FinalImage;
	Settings m_Settings;
	std::vector<float> m_GaussianKernel;

	std::vector<uint32_t> m_ImageHorizontalIterator, m_ImageVerticalIterator;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	glm::vec4* m_HDRImage = nullptr;
	glm::vec4* m_BloomImage = nullptr;
	glm::vec4* m_BlurImage = nullptr;

	uint32_t m_FrameIndex = 1;

	Environment m_Environment;

	std::vector<Light> m_Lights;

	BVH m_BVH;
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