#pragma once

#include <glm/glm.hpp>

namespace BRDF
{
    struct SurfaceData
    {
        glm::vec3 Normal;
        glm::vec3 ViewDirection;
        glm::vec3 LightDirection;
        glm::vec3 HalfVector;
    };

	glm::vec3 FresnelSchlick(float cosTheta, const glm::vec3& F0);
	float DistributionGGX(const glm::vec3& N, const glm::vec3& H, float roughness);

    float GeometrySchlickGGX(float NdotV, float roughness);
	float GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float roughness);

    glm::vec3 ImportanceSampleGGX(const glm::vec2& Xi, const glm::vec3& N, float roughness);

    glm::vec3 EvaluateCookTorrance(const glm::vec3& N, const glm::vec3& V, glm::vec3& L, float roughness, const glm::vec3& F0);

	float PDFGGX(const glm::vec3& N, const glm::vec3& V, const glm::vec3& H, float roughness);
}