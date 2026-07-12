#pragma once
#include <cstdint>
class BVH;
class Scene;

class BVHBuilder
{
public:
	static void Build(BVH& bvh, const Scene& scene);
private:
	static uint32_t BuildRecursive(BVH& bvh, const Scene& scene, uint32_t firstSphere, uint32_t sphereCount);
};