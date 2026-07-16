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
}