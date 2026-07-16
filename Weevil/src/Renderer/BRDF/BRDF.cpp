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
}