#pragma once

#include <glm/glm.hpp>

namespace BRDF
{
    struct GGXSample
    {
        glm::vec3 HalfVector = glm::vec3(0.0f);
    };
    struct GGXEvaluation
    {
		glm::vec3 BRDF = glm::vec3(0.0f);
        float PDF = 0.0f;
    };

    glm::vec3 FresnelSchlick(float cosTheta, const glm::vec3& F0);

    glm::vec3 ComputeDiffuseEnergy(const glm::vec3& fresnel, float metallic);

	float DistributionGGX(const glm::vec3& N, const glm::vec3& H, float roughness);

    float GeometrySchlickGGX(float NdotV, float roughness);
	float GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float roughness);

    GGXSample ImportanceSampleGGX(const glm::vec2& Xi, const glm::vec3& N, const glm::vec3& V, float roughness);
	GGXSample ImportanceSampleGGXVNDF(const glm::vec2& Xi, const glm::vec3& N, const glm::vec3& V, float roughness);

    glm::vec3 EvaluateCookTorrance(const glm::vec3& N, const glm::vec3& V,const glm::vec3& L, const glm::vec3& H,float roughness, const glm::vec3& F0);

	GGXEvaluation EvaluateGGX(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, const glm::vec3& H, float roughness, const glm::vec3& F0);

	float PDFGGX(const glm::vec3& N, const glm::vec3& V, const glm::vec3& H, float roughness);
    
    glm::vec3 CosineSampleHemisphere(const glm::vec2& Xi);
	float CosineHemispherePDF(float NodtL);

	glm::vec3 ToWorld(const glm::vec3& local, const glm::vec3& normal);

    float Average(const glm::vec3& value);
	void ComputeLobeProbabilities(float specularWeight, float& diffuseProbability, float& specularProbability);
}