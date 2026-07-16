#include "InspectorPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void InspectorPanel::Draw(EditorContext& context)
{
    ImGui::Begin("Inspector");

    switch (context.SelectedType)
    {
    case SelectionType::Sphere:
        DrawSphere(context);
        break;

    case SelectionType::Material:
        DrawMaterial(context);
        break;

    case SelectionType::Environment:
        DrawEnvironment(context);
        break;

    default:

        ImGui::TextDisabled("Nothing Selected");

        ImGui::Separator();

        ImGui::TextWrapped(
            "Select an object from the Scene panel.");

        break;
    }

    ImGui::End();
}

void InspectorPanel::DrawSphere(EditorContext& context)
{
    Scene& scene = *context.Scene;
    Renderer& renderer = *context.Renderer;

    if (context.SelectedIndex < 0 ||
        context.SelectedIndex >= (int)scene.Spheres.size())
        return;

    Sphere& sphere = scene.Spheres[context.SelectedIndex];

    ImGui::Text("Sphere %d", context.SelectedIndex);

    ImGui::Separator();

    bool changed = false;

    changed |= ImGui::DragFloat3(
        "Position",
        glm::value_ptr(sphere.Position),
        0.1f);

    changed |= ImGui::DragFloat(
        "Radius",
        &sphere.Radius,
        0.1f);

    changed |= ImGui::DragInt(
        "Material",
        &sphere.MaterialIndex,
        1,
        0,
        (int)scene.Materials.size() - 1);

    if (changed)
        renderer.ResetFrameIndex();
}

void InspectorPanel::DrawMaterial(EditorContext& context)
{
	Scene& scene = *context.Scene;
	Renderer& renderer = *context.Renderer;

	if (context.SelectedIndex < 0 ||
		context.SelectedIndex >= (int)scene.Materials.size())
		return;

	Material& material = scene.Materials[context.SelectedIndex];

	ImGui::Text("Material %d", context.SelectedIndex);

	ImGui::Separator();

	bool changed = false;

	const char* materialTypes[]
	{
		"Diffuse",
		"Metal",
		"Glass"
	};

	int currentType = (int)material.Type;

	if (ImGui::Combo(
		"Material Type",
		&currentType,
		materialTypes,
		IM_ARRAYSIZE(materialTypes)))
	{
		material.Type = (MaterialType)currentType;

		switch (material.Type)
		{
		case MaterialType::Diffuse:
			material.Metallic = 0.0f;
			material.RefractionIndex = 1.52f;
			break;

		case MaterialType::Metal:
			material.RefractionIndex = 1.52f;
			break;

		case MaterialType::Dielectric:
			material.Metallic = 0.0f;
			break;
		}

		changed = true;
	}

	changed |= ImGui::ColorEdit3(
		"Albedo",
		glm::value_ptr(material.Albedo));

	changed |= ImGui::DragFloat(
		"Roughness",
		&material.Roughness,
		0.05f,
		0.0f,
		1.0f);

	if (material.Type == MaterialType::Metal)
	{
		changed |= ImGui::DragFloat(
			"Metallic",
			&material.Metallic,
			0.05f,
			0.0f,
			1.0f);
	}

	if (material.Type == MaterialType::Dielectric)
	{
		changed |= ImGui::DragFloat(
			"IOR",
			&material.RefractionIndex,
			0.01f,
			1.0f,
			2.5f,
			"%.2f");

		changed |= ImGui::ColorEdit3(
			"Transmission Color",
			glm::value_ptr(material.TransmissionColor));

		changed |= ImGui::DragFloat(
			"Absorption Strength",
			&material.AbsorptionStrength,
			0.05f,
			0.0f,
			10.0f);
	}

	ImGui::Separator();

	changed |= ImGui::ColorEdit3(
		"Emission Color",
		glm::value_ptr(material.EmissionColor));

	changed |= ImGui::DragFloat(
		"Emission Power",
		&material.EmissionPower,
		0.1f,
		0.0f,
		FLT_MAX);

	if (changed)
		renderer.ResetFrameIndex();
}

void InspectorPanel::DrawEnvironment(EditorContext& context)
{
    ImGui::Text("Environment");
}