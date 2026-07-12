#include "BVH.h"
#include "BVHTraversal.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Ray.h"
#include "../../Renderer/Geometry/Geometry.h"

#include <array>
#include <cfloat>
#include <cmath>
#include <limits>

HitPayload BVHTraversal::TraceRay(const BVH& bvh, const Scene& scene, const Ray& ray)
{
	HitPayload closestHit{};
	closestHit.HitDistance = std::numeric_limits<float>::max();
	closestHit.ObjectIndex = -1;
	closestHit.MaterialIndex = -1;

	const auto& nodes = bvh.GetNodes();
	if (nodes.empty())
	{
		closestHit.HitDistance = -1.0f;
		return closestHit;
	}

	std::array<uint32_t, 64>stack;
	uint32_t stackPtr = 0;
	stack[stackPtr++] = 0;
	while (stackPtr > 0)
	{
		uint32_t nodeIndex = stack[--stackPtr];
		const BVH::Node& node = nodes[nodeIndex];
		if (!node.Bounds.Intersect(ray.Origin, ray.Direction, 0.001f, closestHit.HitDistance))
		{
			continue;
		}
		if (node.IsLeaf())
		{
			for (uint32_t i = 0; i < node.SphereCount; i++)
			{
				uint32_t sphereIndex = bvh.GetSphereIndices()[node.FirstSphere + i];
				const Sphere& sphere = scene.Spheres[sphereIndex];
				float t;
				if(!Geometry::IntersectSphere(ray, sphere, t))
				{
					continue;
				}
				if(t>= closestHit.HitDistance)
				{
					continue;
				}
				closestHit.HitDistance = t;
				closestHit.ObjectIndex = sphereIndex;
				closestHit.MaterialIndex = sphere.MaterialIndex;
				closestHit.WorldPosition = ray.Origin + ray.Direction * t;
				closestHit.WorldNormal = glm::normalize(closestHit.WorldPosition - sphere.Position);
			}
			continue;
		}

		if(node.LeftChild != UINT32_MAX)
		{
			if (stackPtr < stack.size())
				stack[stackPtr++] = node.LeftChild;
		}
		if(node.RightChild != UINT32_MAX)
		{
			if (stackPtr < stack.size())
				stack[stackPtr++] = node.RightChild;

		}
	}
	if(closestHit.ObjectIndex == -1)
		closestHit.HitDistance = -1.0f;
	return closestHit;
}