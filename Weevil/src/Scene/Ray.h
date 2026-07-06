#pragma once

#include <glm/glm.hpp>

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;

	bool InsideMedium = false;
	glm::vec3 AbsorptionCoefficient = glm::vec3(0.0f);
};
