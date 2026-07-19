#include "../Renderer.h"
#include "../BRDF/BRDF.h"
#include "../BRDF/Fresnel.h"
#include "BSDFSample.h"
#include "Walnut/Random.h"
#include <execution>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

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

BSDFSample Renderer::SampleMetal(const Ray& ray, const HitPayload& payload, const Material& material)
{
    constexpr float kMirrorRoughnessThreshold = 0.001f;
    if (material.Roughness < kMirrorRoughnessThreshold)
        return SampleMirror(ray, payload, material);
	BSDFSample sample;
	glm::vec3 F0(0.04f);
	F0 = glm::mix(F0, material.Albedo, material.Metallic);
	float cosTheta = glm::max(glm::dot(-ray.Direction, payload.WorldNormal), 0.0f);
	glm::vec3 fresnel = BRDF::FresnelSchlick(cosTheta, F0);
	glm::vec2 Xi(Walnut::Random::Float(), Walnut::Random::Float());
	glm::vec3 V = -ray.Direction;
	BRDF::GGXSample ggx = BRDF::ImportanceSampleGGX(Xi,	payload.WorldNormal, V,	material.Roughness);
	glm::vec3 reflected = glm::normalize(ggx.Direction);
	if (glm::dot(reflected, payload.WorldNormal) <= 0.0f)
	{
		reflected =	glm::normalize(glm::reflect(ray.Direction, payload.WorldNormal));
	}
    sample.Direction = reflected;
    // Store the sampled half-vector
    sample.HalfVector = ggx.HalfVector;
    // Evaluate Cook-Torrance BRDF
    glm::vec3 brdf = BRDF::EvaluateCookTorrance(payload.WorldNormal, V, reflected, sample.HalfVector, material.Roughness, F0);
    // Monte Carlo estimator
    float NdotL = glm::max(glm::dot(payload.WorldNormal, reflected), 0.0f);
    if (ggx.PDF > 1e-6f && NdotL > 0.0f)
        sample.Weight = brdf * (NdotL / ggx.PDF);
    else
        sample.Weight = glm::vec3(0.0f);
    sample.PDF = ggx.PDF;
    sample.IsDelta = false;
	return sample;
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