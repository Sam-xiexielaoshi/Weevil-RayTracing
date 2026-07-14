#pragma once
#include "EditorContext.h"
#include "Panels/RendererPanel.h"
#include "Panels/ScenePanel.h"

class EditorUI
{
public:
	void Draw(EditorContext& context);
private:
	RendererPanel m_RendererPanel;
	ScenePanel m_ScenePanel;
};