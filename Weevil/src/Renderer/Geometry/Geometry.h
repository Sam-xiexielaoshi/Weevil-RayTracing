#pragma once
#include "../../Scene/Ray.h"
#include "../../Scene/Scene.h"

namespace Geometry
{
	bool IntersectSphere(const Ray& ray, const Sphere& sphere, float& t);
}