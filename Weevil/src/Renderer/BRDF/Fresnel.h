#pragma once

#include <glm/glm.hpp>

namespace BRDF
{
    glm::vec3 FresnelSchlick(float cosTheta, const glm::vec3& F0);
}