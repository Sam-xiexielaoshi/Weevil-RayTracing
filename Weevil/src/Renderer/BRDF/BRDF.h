#pragma once

#include <glm/glm.hpp>

namespace BRDF
{
    struct GGXSample
    {
        glm::vec3 HalfVector;
        glm::vec3 Direction;
        float PDF = 0.0f;
    };

    glm::vec3 FresnelSchlick(float cosTheta, const glm::vec3& F0);

    glm::vec3 ComputeDiffuseEnergy(const glm::vec3& fresnel, float metallic);

	float DistributionGGX(const glm::vec3& N, const glm::vec3& H, float roughness);

    float GeometrySchlickGGX(float NdotV, float roughness);
	float GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float roughness);

    GGXSample ImportanceSampleGGX(const glm::vec2& Xi, const glm::vec3& N, const glm::vec3& V, float roughness);

    glm::vec3 EvaluateCookTorrance(const glm::vec3& N, const glm::vec3& V,const glm::vec3& L, const glm::vec3& H,float roughness, const glm::vec3& F0);

	float PDFGGX(const glm::vec3& N, const glm::vec3& V, const glm::vec3& H, float roughness);
}