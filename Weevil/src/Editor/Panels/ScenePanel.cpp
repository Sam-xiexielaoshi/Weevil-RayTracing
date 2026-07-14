#include "ScenePanel.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void ScenePanel::Draw(EditorContext& context)
{
	ImGui::Begin("Scene");
	DrawSpheres(context);
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