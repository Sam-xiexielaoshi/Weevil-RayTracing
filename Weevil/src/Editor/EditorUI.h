#pragma once
#include "EditorContext.h"

#include "Core/PanelManager.h"

#include "Panels/RendererPanel.h"
#include "Panels/ScenePanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/MenuBarPanel.h"

class EditorUI
{
public:
	EditorUI();
	void Draw(EditorContext& context);
private:
	PanelManager m_PanelManager;
};