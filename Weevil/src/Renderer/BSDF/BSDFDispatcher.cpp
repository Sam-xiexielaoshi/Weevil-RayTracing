#include "../Renderer.h"

BSDFSample Renderer::SampleBSDF(Ray& ray, const HitPayload& payload, const Material& material)
{
	switch (material.Type)
	{
	case MaterialType::Diffuse:
		return SampleDiffuse(ray, payload, material);

	case MaterialType::Metal:
		return SampleMetal(ray, payload, material);

	case MaterialType::Dielectric:
		return SampleDielectric(ray, payload, material);

	default:
		return {};
	}
}