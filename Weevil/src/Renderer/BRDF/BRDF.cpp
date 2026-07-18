#include "BRDF.h"
#include <glm/gtc/constants.hpp>
#include <cmath>

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
    glm::vec3 ImportanceSampleGGX(const glm::vec2& Xi, const glm::vec3& N, float roughness)
    {
		float a = roughness * roughness;
		float phi = 2.0f * glm::pi<float>() * Xi.x;
		float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
        float sinTheta = sqrt(glm::max(0.0f, 1.0f - cosTheta * cosTheta));
        glm::vec3 H;
		H.x = cos(phi) * sinTheta;
		H.y = sin(phi) * sinTheta;
		H.z = cosTheta;
        glm::vec3 up = std::abs(N.z) < 0.999f ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 tangent = glm::normalize(glm::cross(up, N));
		glm::vec3 bitangent = glm::cross(N, tangent);
		glm::vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
        return glm::normalize(sampleVec);
    }
}