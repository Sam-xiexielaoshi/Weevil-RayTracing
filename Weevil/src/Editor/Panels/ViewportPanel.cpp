#include "ViewportPanel.h"
#include <imgui.h>

void ViewportPanel::Draw(EditorContext& context)
{
	Renderer& renderer = *context.Renderer;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport");

	*context.ViewportWidth = ImGui::GetContentRegionAvail().x;
	*context.ViewportHeight = ImGui::GetContentRegionAvail().y;

	auto image = renderer.GetFinalImage();
	if (image)
		ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), 
			(float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
	ImGui::PopStyleVar();
}