#include "BVH.h"
#include "BVHBuilder.h"
#include "BVHTraversal.h"

void BVH::Build(const Scene& scene)
{
	BVHBuilder::Build(*this, scene);
}

HitPayload BVH::TraceRay(const Scene& scene, const Ray& ray) const
{
	return BVHTraversal::TraceRay(*this, scene, ray);
}
