#include "../Renderer.h"
#include "../BRDF/BRDF.h"
#include "../BRDF/Fresnel.h"
#include "BSDFSample.h"
#include "Walnut/Random.h"
#include <execution>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

BSDFSample Renderer::SampleBSDF(Ray& ray, const HitPayload& payload, const Material& material)
{
    switch (material.Type)
    {
    case MaterialType::Diffuse:
    {
		constexpr float kDielectricSpecularWeight = 0.08f;
        float diffuseProbability;
		float specularProbability;
        BRDF::ComputeLobeProbabilities(kDielectricSpecularWeight, diffuseProbability, specularProbability);
        if (Walnut::Random::Float() < diffuseProbability)
        {
            BSDFSample sample = SampleDiffuse(ray, payload, material);
			sample.SelectionPDF = diffuseProbability;
            return sample;
        }
		BSDFSample sample = SampleSpecularDielectric(ray, payload, material);
		sample.SelectionPDF = specularProbability;
        return sample;
    }

    case MaterialType::Metal:
        return SampleMetal(ray, payload, material);

    case MaterialType::Dielectric:
        return SampleDielectric(ray, payload, material);

    default:
        return {};
    }
}

BSDFSample Renderer::SampleDiffuse(const Ray& ray,const HitPayload& payload, const Material& material)
{
	BSDFSample sample;
	
	glm::vec2 Xi = glm::vec2(Walnut::Random::Float(), Walnut::Random::Float());
    glm::vec3 local = BRDF::CosineSampleHemisphere(Xi);
	glm::vec3 direction = BRDF::ToWorld(local, payload.WorldNormal);

	sample.Direction = glm::normalize(direction);
    glm::vec3 V = -ray.Direction;
    float cosTheta = glm::max(glm::dot(V, payload.WorldNormal), 0.0f);
    glm::vec3 F0(0.04f);
    F0 = glm::mix(F0, material.Albedo, material.Metallic);
    glm::vec3 fresnel = BRDF::FresnelSchlick(cosTheta, F0);
    glm::vec3 kD = BRDF::ComputeDiffuseEnergy(fresnel, material.Metallic);
    
    float NdotL = glm::max(glm::dot(payload.WorldNormal, sample.Direction), 0.0f);
	glm::vec3 brdf = (material.Albedo / glm::pi<float>()) * kD;
	sample.PDF = BRDF::CosineHemispherePDF(NdotL);
    if(sample.PDF>1e-6f)
        sample.Weight = brdf * (NdotL / sample.PDF);
    else
		sample.Weight = glm::vec3(0.0f);

	return sample;
}

BSDFSample Renderer::SampleGGX(const Ray& ray, const HitPayload& payload, float roughness, const glm::vec3& F0)
{
    BSDFSample sample;
	glm::vec3 N = payload.WorldNormal;
    glm::vec3 V = -ray.Direction;
	glm::vec2 Xi = glm::vec2(Walnut::Random::Float(), Walnut::Random::Float());
	BRDF::GGXSample ggx = BRDF::ImportanceSampleGGX(Xi, N, V, roughness);
    glm::vec3 H = ggx.HalfVector;
	glm::vec3 L = glm::normalize(glm::reflect(-V, H));
    if (glm::dot(N, L) <= 0.0f)
    {
        L = glm::normalize(glm::reflect(ray.Direction, N));
        H = glm::normalize(V + L);
    }
	float pdf = BRDF::PDFGGX(N, V, H, roughness);
	glm::vec3 brdf = BRDF::EvaluateCookTorrance(N, V, L, H, roughness, F0);
	float NdotL = glm::max(glm::dot(N, L), 0.0f);
    sample.Direction = L;
    sample.HalfVector = H;
	sample.PDF = pdf;
	sample.IsDelta = false;
    if (pdf > 1e-6f)
    {
        sample.Weight = brdf * (NdotL / pdf);
    }
    else
    {
        sample.Weight = glm::vec3(0.0f);
    }
    return sample;
}

BSDFSample Renderer::SampleMetal(const Ray& ray, const HitPayload& payload, const Material& material)
{
	constexpr float kMirrorThreshold = 0.001f;
    if (material.Roughness < kMirrorThreshold)
        return SampleMirror(ray, payload, material);
	glm::vec3 F0 = glm::mix(glm::vec3(0.04f), material.Albedo, material.Metallic);
	return SampleGGX(ray, payload, material.Roughness, F0);
}

BSDFSample Renderer::SampleSpecularDielectric(const Ray& ray, const HitPayload& payload, const Material& material)
{
    constexpr float kMirrorThreshold = 0.001f;

    if (material.Roughness < kMirrorThreshold)
        return SampleDielectricMirror(ray, payload);

    return SampleGGX(ray, payload, material.Roughness, glm::vec3(0.04f));
}

BSDFSample Renderer::SampleDielectric(Ray& ray, const HitPayload& payload, const Material& material)
{
    BSDFSample sample;
    bool frontFace = glm::dot(ray.Direction, payload.WorldNormal) < 0.0f;
    glm::vec3 normal = frontFace ? payload.WorldNormal : -payload.WorldNormal;
    float eta = frontFace ? (1.0f / material.RefractionIndex) : material.RefractionIndex;
    float cosTheta = glm::min(glm::dot(-ray.Direction, normal), 1.0f);
    float sinTheta = glm::sqrt(1.0f - cosTheta * cosTheta);
    bool cannotRefract = eta * sinTheta > 1.0f;
    glm::vec3 F0 = glm::vec3(std::pow((1.0f - material.RefractionIndex) / (1.0f + material.RefractionIndex), 2.0f));
    float reflectProbability = BRDF::FresnelSchlick(cosTheta, F0).r;
    if (frontFace)
    {
        ray.InsideMedium = true;
        ray.AbsorptionCoefficient = (glm::vec3(1.0f) - material.TransmissionColor) * material.AbsorptionStrength;
    }
    else
    {
        ray.InsideMedium = false;
        ray.AbsorptionCoefficient = glm::vec3(0.0f);
    }

    if (cannotRefract || Walnut::Random::Float() < reflectProbability)
    {
        sample.Direction = glm::normalize(glm::reflect(ray.Direction, normal));
    }
    else
    {
        sample.Direction = glm::normalize(glm::refract(ray.Direction, normal, eta));
    }
    // Temporary placeholder
    sample.Weight = glm::vec3(1.0f);
    sample.PDF = 1.0f;
    sample.IsDelta = true;
    return sample;
}

BSDFSample Renderer::SampleMirror(const Ray& ray, const HitPayload& payload, const Material& material)
{
    BSDFSample sample;
    glm::vec3 reflected = glm::normalize(glm::reflect(ray.Direction, payload.WorldNormal));
    glm::vec3 V = -ray.Direction;
    glm::vec3 F0(0.04f);
    F0 = glm::mix(F0, material.Albedo, material.Metallic);
    float cosTheta = glm::max(glm::dot(V, payload.WorldNormal), 0.0f);
    glm::vec3 fresnel = BRDF::FresnelSchlick(cosTheta, F0);
    sample.Direction = reflected;
    sample.Weight = fresnel;
    // Delta distribution
    sample.PDF = 1.0f;
    sample.IsDelta = true;
    return sample;
}

BSDFSample Renderer::SampleDielectricMirror(const Ray& ray, const HitPayload& payload)
{
    BSDFSample sample;
    glm::vec3 reflected = glm::normalize(glm::reflect(ray.Direction, payload.WorldNormal));
    glm::vec3 V = -ray.Direction;

    constexpr glm::vec3 F0(0.04f);

    float cosTheta =glm::max(glm::dot(V, payload.WorldNormal), 0.0f);

    glm::vec3 fresnel = BRDF::FresnelSchlick(cosTheta, F0);

    sample.Direction = reflected;
    sample.Weight = fresnel;

    sample.PDF = 1.0f;
    sample.IsDelta = true;

    return sample;
}

bool Renderer::RussianRoulette(glm::vec3& throughput, int bounce)
{
	//russain roulette termination
	if (!m_Settings.EnableRussianRoulette)
		return true;
	if (bounce < 2)
		return true;

	float survival = glm::max(throughput.r, glm::max(throughput.g, throughput.b));
	survival = glm::clamp(survival, 0.05f, 0.95f);

	if (Walnut::Random::Float() > survival)
		return false;
	throughput /= survival;
	return true;
}