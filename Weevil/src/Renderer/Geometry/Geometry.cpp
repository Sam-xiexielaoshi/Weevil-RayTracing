#include "Geometry.h"
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>


namespace Geometry
{
	bool IntersectSphere(const Ray& ray, const Sphere& sphere, float& t)
	{
		glm::vec3 oc = ray.Origin - sphere.Position;
		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(oc, ray.Direction);
		float c = glm::dot(oc, oc) - sphere.Radius * sphere.Radius;
		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0.0f)
		{
			return false;
		}
		float sqrtDiscriminant = std::sqrt(discriminant);
		float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
		float t1 = (-b + sqrtDiscriminant) / (2.0f * a);
		if (t0 > 0.001f)
			t = t0;
		else
			t = t1;
		return t > 0.001f;
	}
}