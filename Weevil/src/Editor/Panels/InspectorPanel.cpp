#include "InspectorPanel.h"
#include <imgui.h>

void InspectorPanel::Draw(EditorContext& context)
{
	ImGui::Begin("Inspector");
	ImGui::TextDisabled("Nothing Selected");
	ImGui::Separator();
	ImGui::TextWrapped("Select an object in the scene to view its properties here.");
	ImGui::End();
}