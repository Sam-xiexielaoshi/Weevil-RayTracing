#include "MenuBarPanel.h"
#include "../Commands/EditorCommands.h"
#include <imgui.h>

void MenuBarPanel::Draw(EditorContext& context)
{
	if(!ImGui::BeginMainMenuBar())
		return;
	//file
	if (ImGui::BeginMenu("File"))
	{
		ImGui::MenuItem("New Scene", nullptr, false, false);
		ImGui::MenuItem("Open Scene", nullptr, false, false);
		ImGui::MenuItem("Save Scene", nullptr, false, false);
		ImGui::Separator();	
		if (ImGui::MenuItem("Exit"))
		{
			//todo
		}
		ImGui::EndMenu();
	}
	//edit
	if (ImGui::BeginMenu("Edit"))
	{
		ImGui::MenuItem("Undo", "Ctrl+Z", false, false);
		ImGui::MenuItem("Redo", "Ctrl+Y", false, false);
		ImGui::EndMenu();
	}
	//create
	if(ImGui::BeginMenu("Create"))
	{
		if (ImGui::MenuItem("Sphere"))
		{
			EditorCommands::CreateSphere(context);
		}
		if (ImGui::MenuItem("Material"))
		{
			EditorCommands::CreateMaterial(context);
		}
		ImGui::EndMenu();
	}
	//view
	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("Renderer", nullptr, true, false);
		ImGui::MenuItem("Scene", nullptr, true, false);
		ImGui::MenuItem("Inspector", nullptr, true, false);
		ImGui::MenuItem("Viewport", nullptr, true, false);
		ImGui::EndMenu();
	}
	//Help
	if(ImGui::BeginMenu("Help"))
	{
		ImGui::TextDisabled("Weevil Engine");
		ImGui::Separator();
		ImGui::Text("Version 0.1");
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}