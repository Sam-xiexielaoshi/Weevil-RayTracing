#include "EditorUI.h"

void EditorUI::Draw(EditorContext& context)
{
	m_MenuBarPanel.Draw(context);
	m_RendererPanel.Draw(context);
	m_ScenePanel.Draw(context);
	m_InspectorPanel.Draw(context);
	m_ViewportPanel.Draw(context);
}