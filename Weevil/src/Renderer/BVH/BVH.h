#pragma once
#include "../Geometry/AABB.h"
#include <vector>
#include <cstdint>
#include "../Integrator/HitPayload.h"

struct Ray;
class Scene;
class BVHTraversal;

class BVH
{
public:
	struct Node
	{
		AABB Bounds;
		uint32_t LeftChild = UINT32_MAX;
		uint32_t RightChild = UINT32_MAX;
		uint32_t FirstSphere = 0;
		uint32_t SphereCount = 0;
		bool IsLeaf() const{return SphereCount > 0;}
	};

	HitPayload TraceRay(const Scene& scene, const Ray& ray) const;

public:
	void Build(const Scene& scene);
	const std::vector<Node>& GetNodes() const { return m_Nodes; }
	const std::vector<uint32_t>& GetSphereIndices() const { return m_SphereIndices; }
private:
	std::vector<uint32_t>m_SphereIndices;
	std::vector<Node> m_Nodes;
	friend class BVHBuilder;
};

