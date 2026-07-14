#include "EditorUI.h"

void EditorUI::Draw(EditorContext& context)
{
	m_RendererPanel.Draw(context);
	m_ScenePanel.Draw(context);
}