#pragma once
#include <cstdint>

class BVH;
class Scene;
class Ray;

struct Sphere;
struct HitPayload;

class BVHTraversal
{
public:
	static HitPayload TraceRay(const BVH& bvh, const Scene& scene, const Ray& ray);
};