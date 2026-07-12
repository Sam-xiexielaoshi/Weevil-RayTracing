#include "BVHBuilder.h"
#include "BVH.h"
#include <algorithm>

#include "../../Scene/Scene.h"

namespace
{
	constexpr uint32_t MaxLeafSize = 2;
}

void BVHBuilder::Build(BVH& bvh, const Scene& scene)
{
	bvh.m_Nodes.clear();
	bvh.m_SphereIndices.clear();
	const uint32_t sphereCount = static_cast<uint32_t>(scene.Spheres.size());
	bvh.m_SphereIndices.resize(sphereCount);
	for (uint32_t i = 0; i < sphereCount; i++)
		bvh.m_SphereIndices[i] = i;

	BuildRecursive(bvh, scene, 0, sphereCount);
}

uint32_t BVHBuilder::BuildRecursive(BVH& bvh, const Scene& scene, uint32_t firstSphere, uint32_t sphereCount)
{
    uint32_t nodeIndex = static_cast<uint32_t>(bvh.m_Nodes.size());
    bvh.m_Nodes.emplace_back();
    bvh.m_Nodes[nodeIndex].FirstSphere = firstSphere;
    bvh.m_Nodes[nodeIndex].SphereCount = sphereCount;

    AABB bounds;

    for (uint32_t i = 0; i < sphereCount; i++)
    {
        uint32_t sphereIndex = bvh.m_SphereIndices[firstSphere + i];
        const Sphere& sphere = scene.Spheres[sphereIndex];
        bounds.Expand(sphere.Position - glm::vec3(sphere.Radius));
        bounds.Expand(sphere.Position + glm::vec3(sphere.Radius));
    }
    bvh.m_Nodes[nodeIndex].Bounds = bounds;

    if (sphereCount <= MaxLeafSize)
    {
        return nodeIndex;
    }

    glm::vec3 extents = bounds.Extents();
    int axis = 0;
    if (extents.y > extents.x)
        axis = 1;
    if (extents.z > extents[axis])
        axis = 2;

    std::sort(
        bvh.m_SphereIndices.begin() + firstSphere,
        bvh.m_SphereIndices.begin() + firstSphere + sphereCount,
        [&](uint32_t a, uint32_t b)
        {
            return scene.Spheres[a].Position[axis] < scene.Spheres[b].Position[axis];
        });
    uint32_t leftCount = sphereCount / 2;
    uint32_t rightCount = sphereCount - leftCount;

    bvh.m_Nodes[nodeIndex].FirstSphere = 0;
    bvh.m_Nodes[nodeIndex].SphereCount = 0;

    bvh.m_Nodes[nodeIndex].LeftChild = BuildRecursive(bvh, scene, firstSphere, leftCount);

    bvh.m_Nodes[nodeIndex].RightChild = BuildRecursive(bvh, scene, firstSphere + leftCount, rightCount);
    return nodeIndex;
}
