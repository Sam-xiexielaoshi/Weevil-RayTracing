#include "AABB.h"
#include <algorithm>

AABB::AABB()
{
	Min = glm::vec3(std::numeric_limits<float>::max());
	Max = glm::vec3(-std::numeric_limits<float>::max());
}

void AABB::Expand(const glm::vec3& point)
{
	Min = glm::min(Min, point);
	Max = glm::max(Max, point);
}

void AABB::Expand(const AABB& box)
{
	Expand(box.Min);
	Expand(box.Max);
}

glm::vec3 AABB::Center() const
{
	return (Min + Max) * 0.5f;
}

glm::vec3 AABB::Extents() const
{
	return Max - Min;
}

float AABB::SurfaceArea() const
{
	glm::vec3 extents = Extents();
	return 2.0f * (extents.x * extents.y + extents.x * extents.z + extents.y * extents.z);
}

bool AABB::Intersect(const glm::vec3& origin, const glm::vec3& direction, float tMin, float tMax) const
{
    for (int axis = 0; axis < 3; axis++)
    {
        float invD = 1.0f / direction[axis];
        float t0 = (Min[axis] - origin[axis]) * invD;
        float t1 = (Max[axis] - origin[axis]) * invD;

        if (invD < 0.0f)
            std::swap(t0, t1);
        tMin = glm::max(tMin, t0);
        tMax = glm::min(tMax, t1);
        if (tMax < tMin)
            return false;
    }
    return true;
}
