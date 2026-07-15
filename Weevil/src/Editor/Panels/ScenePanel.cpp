#include "ScenePanel.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void ScenePanel::Draw(EditorContext& context)
{
	ImGui::Begin("Scene");
	DrawSpheres(context);
	DrawMaterials(context);
	DrawEnvironment(context);
	ImGui::End();
}

void ScenePanel::DrawSpheres(EditorContext& context)
{
	Scene& scene = *context.Scene;
	Renderer& renderer = *context.Renderer;
	//spheres tree node
	if (ImGui::TreeNodeEx("Spheres", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (size_t i = 0; i < scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);
			std::string label = "Sphere " + std::to_string(i);
			if (ImGui::TreeNode(label.c_str()))
			{
				Sphere& sphere = scene.Spheres[i];
				bool sphereChanged = false;
				sphereChanged |= ImGui::DragFloat3(
					"Position",
					glm::value_ptr(sphere.Position),
					0.1f);
				sphereChanged |= ImGui::DragFloat(
					"Radius",
					&sphere.Radius,
					0.1f);
				sphereChanged |= ImGui::DragInt(
					"Material",
					&sphere.MaterialIndex,
					1,
					0,
					(int)scene.Materials.size() - 1);
				if (sphereChanged)
					renderer.ResetFrameIndex();
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
}

void ScenePanel::DrawMaterials(EditorContext& context)
{
	Scene& scene = *context.Scene;
	Renderer& renderer = *context.Renderer;
	if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (size_t i = 0; i < scene.Materials.size(); i++)
		{
			ImGui::PushID(i);
			std::string label = "Material " + std::to_string(i);
			if (ImGui::TreeNode(label.c_str()))
			{
				Material& material = scene.Materials[i];
				bool materialChanged = false;
				// Material Type
				const char* materialTypes[]
				{
					"Diffuse",
					"Metal",
					"Glass"
				};
				int currentType = static_cast<int>(material.Type);
				if (ImGui::Combo(
					"Material Type",
					&currentType,
					materialTypes,
					IM_ARRAYSIZE(materialTypes)))
				{
					material.Type = static_cast<MaterialType>(currentType);
					// Reset properties that don't belong
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
					materialChanged = true;
				}
				// Surface
				materialChanged |= ImGui::ColorEdit3(
					"Albedo",
					glm::value_ptr(material.Albedo));

				materialChanged |= ImGui::DragFloat(
					"Roughness",
					&material.Roughness,
					0.05f,
					0.0f,
					1.0f);

				if (material.Type == MaterialType::Metal)
				{
					materialChanged |= ImGui::DragFloat(
						"Metallic",
						&material.Metallic,
						0.05f,
						0.0f,
						1.0f);
				}

				if (material.Type == MaterialType::Dielectric)
				{
					materialChanged |= ImGui::DragFloat(
						"IOR",
						&material.RefractionIndex,
						0.01f,
						1.0f,
						2.5f,
						"%.2f");

					materialChanged |= ImGui::ColorEdit3(
						"Transmission Color",
						glm::value_ptr(material.TransmissionColor));

					materialChanged |= ImGui::DragFloat(
						"Absorption Strength",
						&material.AbsorptionStrength,
						0.05f,
						0.0f,
						10.0f);
				}
				ImGui::Separator();
				// Emission

				materialChanged |= ImGui::ColorEdit3(
					"Emission Color",
					glm::value_ptr(material.EmissionColor));

				materialChanged |= ImGui::DragFloat(
					"Emission Power",
					&material.EmissionPower,
					0.1f,
					0.0f,
					FLT_MAX);

				if (materialChanged)
					renderer.ResetFrameIndex();
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}

void ScenePanel::DrawEnvironment(EditorContext& context)
{
	Renderer& renderer = *context.Renderer;
	//materials tree node
	if (ImGui::TreeNodeEx("Environment", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool changed = false;

		static float exposure = 1.0f;
		static float rotation = 0.0f;

		changed |= ImGui::DragFloat(
			"Exposure",
			&exposure,
			0.05f,
			0.0f,
			10.0f);

		changed |= ImGui::DragFloat(
			"Rotation",
			&rotation,
			1.0f,
			0.0f,
			360.0f);

		if (changed)
		{
			renderer.GetEnvironment().SetExposure(exposure);
			renderer.GetEnvironment().SetRotation(rotation);

			renderer.ResetFrameIndex();
		}

		ImGui::TreePop();
	}
}

