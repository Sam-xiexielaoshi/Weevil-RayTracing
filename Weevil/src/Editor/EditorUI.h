#pragma once
#include "EditorContext.h"


#include "Panels/RendererPanel.h"
#include "Panels/ScenePanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/MenuBarPanel.h"

class EditorUI
{
public:
	void Draw(EditorContext& context);
private:
	MenuBarPanel m_MenuBarPanel;

	RendererPanel m_RendererPanel;
	ScenePanel m_ScenePanel;
	ViewportPanel m_ViewportPanel;
	InspectorPanel m_InspectorPanel;
};