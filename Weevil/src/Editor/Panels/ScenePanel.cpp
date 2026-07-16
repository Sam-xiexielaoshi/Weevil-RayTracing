#include "ScenePanel.h"
#include "../Commands/EditorCommands.h"
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
			bool open = ImGui::TreeNode(label.c_str());
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete"))
				{
					EditorCommands::DeleteSphere(context, static_cast<int>(i));
					ImGui::EndPopup();
					if (open) ImGui::TreePop();
					ImGui::PopID();
					break;
				}
				ImGui::EndPopup();
			}
			if (ImGui::IsItemClicked())
			{
				context.SelectedType = SelectionType::Sphere;
				context.SelectedIndex = static_cast<int>(i);
			}
			if (open)
			{
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
			bool open = ImGui::TreeNode(label.c_str());
			if (ImGui::IsItemClicked())
			{
				context.SelectedType = SelectionType::Material;
				context.SelectedIndex = static_cast<int>(i);
			}
			if (open)
			{
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
		if (ImGui::IsItemClicked())
		{
			context.SelectedType = SelectionType::Environment;
			context.SelectedIndex = 0;
		}
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
