#include "../Renderer.h"
#include "HitPayload.h"
#include "../Geometry/Geometry.h"

HitPayload Renderer::TraceRay(const Ray& ray)
{
	if (m_Settings.UseBVH)
		return m_BVH.TraceRay(*m_ActiveScene, ray);

	return TraceRayBruteForce(ray);
}

void Renderer::OffsetRayOrigin(Ray& ray, const HitPayload& payload)
{
	constexpr float bias = 0.0001f;
	ray.Origin = payload.WorldPosition + ray.Direction * bias;
}

HitPayload Renderer::TraceRayBruteForce(const Ray& ray)
{
	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		float t;
		if(!Geometry::IntersectSphere(ray, sphere, t))
			continue;
		if (t < hitDistance)
		{
			hitDistance = t;
			closestSphere = static_cast<int>(i);
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);
}

HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition =
		ray.Origin + hitDistance * ray.Direction;

	payload.WorldNormal =
		glm::normalize(payload.WorldPosition - closestSphere.Position);

	return payload;
}

HitPayload Renderer::Miss(const Ray& ray)
{
	HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}

bool Renderer::IsOccluded(const Ray& shadowRay, float maxDistance, int ignoredSphere)
{
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		if (static_cast<int>(i) == ignoredSphere) continue;
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		float t;
		if (Geometry::IntersectSphere(shadowRay, sphere, t) && t < maxDistance)
			return true;
	}
	return false;
}
