#include "EditorUI.h"

EditorUI::EditorUI()
{
    m_PanelManager.AddPanel<MenuBarPanel>();
    m_PanelManager.AddPanel<RendererPanel>();
    m_PanelManager.AddPanel<ScenePanel>();
    m_PanelManager.AddPanel<InspectorPanel>();
    m_PanelManager.AddPanel<ViewportPanel>();
}

void EditorUI::Draw(EditorContext& context)
{
    m_PanelManager.Draw(context);
}