#pragma once

#include <glm/glm.hpp>
#include <vector>

enum class MaterialType
{
	Diffuse = 0,
	Metal, 
	Dielectric
};

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;

	MaterialType Type = MaterialType::Diffuse;

	float RefractionIndex = 1.52f; //for dielectric materials

	glm::vec3 EmissionColor{ 0.0f };
	float EmissionPower = 0.0f;

	glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }

	glm::vec3 TransmissionColor = glm::vec3(1.0f);
	float AbsorptionStrength = 0.0f;
};
struct Sphere
{
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere>Spheres;
	std::vector<Material>Materials;
};