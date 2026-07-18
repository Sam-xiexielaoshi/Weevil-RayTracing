#pragma once
#include "../BRDF/BRDF.h"
#include <glm/glm.hpp>

struct BSDFSample
{
	//sampled outgoing
	glm::vec3 Direction = glm::vec3(0.0f);

	//.brdf*costheta / pdf
	glm::vec3 Weight = glm::vec3(1.0f);
	
	glm::vec3 HalfVector = glm::vec3(0.0f);

	//sample prob
	float PDF = 0.0f;

	//perfectr specular
	bool IsDelta = false;
};