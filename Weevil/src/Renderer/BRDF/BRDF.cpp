#include "BRDF.h"
#include <glm/gtc/constants.hpp>

namespace BRDF
{
    float DistributionGGX(const glm::vec3& N, const glm::vec3& H, float roughness)
    {
        float a = roughness * roughness;
        float a2 = a * a;

        float NdotH = glm::max(glm::dot(N, H), 0.0f);
        float NdotH2 = NdotH * NdotH;

        float denominator = NdotH2 * (a2 - 1.0f) + 1.0f;
        denominator = glm::pi<float>() * denominator * denominator;
        return a2 / glm::max(denominator, 0.000001f);
    }
    float GeometrySchlickGGX(float NdotV, float roughness)
    {
		float r = roughness + 1.0f;
		float k = (r * r) / 8.0f;
		float denominator = NdotV * (1.0f - k) + k;
        return NdotV / glm::max(denominator, 0.000001f);
    }
    float GeometrySmith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float roughness)
    {
        float NdotV = glm::max(glm::dot(N, V), 0.0f);
		float NdotL = glm::max(glm::dot(N, L), 0.0f);
		float ggx1 = GeometrySchlickGGX(NdotV, roughness);
		float ggx2 = GeometrySchlickGGX(NdotL, roughness);
        return ggx1 * ggx2;
    }
}