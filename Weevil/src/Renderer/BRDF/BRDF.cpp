#include "BRDF.h"
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace BRDF
{
    float DistributionGGX(const glm::vec3& N, const glm::vec3& H, float roughness)
    {
        const float MIN_ROUGHNESS = 0.001f;

        float a = glm::max(roughness, MIN_ROUGHNESS);
        a *= a;
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
    GGXSample ImportanceSampleGGX(const glm::vec2& Xi, const glm::vec3& N, const glm::vec3& V, float roughness)
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
        GGXSample sample;
		sample.HalfVector = glm::normalize(sampleVec);
		sample.Direction = glm::reflect(-V, sample.HalfVector);
		sample.Direction = glm::normalize(sample.Direction);
		sample.PDF = PDFGGX(N, V, sample.HalfVector, roughness);
        return sample;
    }
    glm::vec3 EvaluateCookTorrance(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, const glm::vec3& H, float roughness, const glm::vec3& F0)
    {
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        glm::vec3 F = FresnelSchlick(glm::max(glm::dot(H, V), 0.0f), F0);
        float NdotV = glm::max(glm::dot(N, V), 0.0f);
        float NdotL = glm::max(glm::dot(N, L), 0.0f);
        float denominator = 4.0f * NdotV * NdotL + 0.000001f;
        return (D * G * F) / denominator;
    }
    float PDFGGX(const glm::vec3& N, const glm::vec3& V, const glm::vec3& H, float roughness)
    {
		float D = DistributionGGX(N, H, roughness);
		float NdotH = glm::max(glm::dot(N, H), 0.0f);
		float VdotH = glm::max(glm::dot(V, H), 0.0f);
        if (VdotH < 1e-6f)
            return 0.0f;
		return (D * NdotH) / (4.0f * VdotH);
    }
    glm::vec3 CosineSampleHemisphere(const glm::vec2& Xi)
    {
        float r = glm::sqrt(Xi.x);
		float theta = 2.0f * glm::pi<float>() * Xi.y;
		float x = r * glm::cos(theta);
		float y = r * glm::sin(theta);
		float z = glm::sqrt(glm::max(0.0f, 1.0f - Xi.x));
        return glm::vec3(x,y,z);
    }

    float CosineHemispherePDF(float NdotL)
    {
        return NdotL / glm::pi<float>();
    }

    glm::vec3 ToWorld(const glm::vec3& local, const glm::vec3& normal)
    {
        glm::vec3 up = glm::abs(normal.z) < 0.999f ? glm::vec3(0, 0, 1) : glm::vec3(1, 0, 0);
		glm::vec3 tangent = glm::normalize(glm::cross(up, normal));
		glm::vec3 bitangent = glm::cross(normal, tangent);
		return glm::normalize(local.x * tangent + local.y * bitangent + local.z * normal);
    }

}