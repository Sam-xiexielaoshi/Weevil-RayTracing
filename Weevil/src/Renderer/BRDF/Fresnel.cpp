#include "Fresnel.h"

#include <cmath>
#include "BRDF.h"

namespace BRDF
{
    glm::vec3 FresnelSchlick(float cosTheta, const glm::vec3& F0)
    {
        return F0 + (glm::vec3(1.0f) - F0) * std::pow(1.0f - cosTheta, 5.0f);
    }
    glm::vec3 BRDF::ComputeDiffuseEnergy(const glm::vec3& fresnel, float metallic)
    {
        //specular energy
        glm::vec3 kS = fresnel;
        //Remaing energy
        glm::vec3 kD = glm::vec3(1.0f) - kS;
        //metals have no diffuce lobe
		kD *= (1.0f - metallic);
        return kD;
    }
}