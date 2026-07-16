#include "Fresnel.h"

#include <cmath>

namespace BRDF
{
    glm::vec3 FresnelSchlick(float cosTheta, const glm::vec3& F0)
    {
        return F0 + (glm::vec3(1.0f) - F0) * std::pow(1.0f - cosTheta, 5.0f);
    }
}