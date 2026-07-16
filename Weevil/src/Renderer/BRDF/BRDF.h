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
}