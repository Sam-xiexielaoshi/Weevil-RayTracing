#include "Renderer.h"

void Renderer::OffsetRayOrigin(Ray& ray, const HitPayload& payload)
{
	constexpr float bias = 0.0001f;
	ray.Origin = payload.WorldPosition + ray.Direction * bias;
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		glm::vec3 origin = ray.Origin - sphere.Position; //origin is the vector from the ray origin to the sphere center

		float a = glm::dot(ray.Direction, ray.Direction); //ray origin
		float b = 2.0f * glm::dot(origin, ray.Direction); //ray direction
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius; //since we are assuming the sphere center is at the origin we can simplify the eq to this
		//quad eq formual discriminant = b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f)
			continue;

		constexpr float tMin = 0.001f;
		float sqrtD = glm::sqrt(discriminant);
		float t0 = (-b - sqrtD) / (2.0f * a);
		float t1 = (-b + sqrtD) / (2.0f * a);

		float t = t0;

		if (t < tMin) t = t1;
		if (t > tMin && t < hitDistance)
		{
			hitDistance = t;
			closestSphere = static_cast<int>(i);
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
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

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}

bool Renderer::IsOccluded(const Ray& shadowRay, float maxDistance, int ignoredSphere)
{
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		if (static_cast<int>(i) == ignoredSphere) continue;
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		glm::vec3 origin = shadowRay.Origin - sphere.Position;

		float a = glm::dot(shadowRay.Direction, shadowRay.Direction);
		float b = 2.0f * glm::dot(origin, shadowRay.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f) continue;

		float sqrtD = sqrt(discriminant);
		float t0 = (-b - sqrtD) / (2.0f * a);
		float t1 = (-b + sqrtD) / (2.0f * a);

		if (t0 > 0.001f && t0 < maxDistance) return true;
		if (t1 > 0.001f && t1 < maxDistance) return true;
	}
	return false;
}
