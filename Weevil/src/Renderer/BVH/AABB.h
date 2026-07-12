#pragma once
#include <glm/glm.hpp>
#include <limits>

struct AABB
{
	glm::vec3 Min, Max;

	AABB();

	void Expand(const glm::vec3& point);
	void Expand(const AABB& box);

	glm::vec3 Center() const;
	glm::vec3 Extents() const;

	float SurfaceArea() const;
	bool Intersect(const glm::vec3& origin, const glm::vec3& direction, float tMin, float tMax) const;
};
