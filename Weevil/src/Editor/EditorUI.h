#pragma once
#include "EditorContext.h"

#include "Panels/RendererPanel.h"
#include "Panels/ScenePanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/InspectorPanel.h"

class EditorUI
{
public:
	void Draw(EditorContext& context);
private:
	RendererPanel m_RendererPanel;
	ScenePanel m_ScenePanel;
	ViewportPanel m_ViewportPanel;
	InspectorPanel m_InspectorPanel;
};