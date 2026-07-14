#pragma once
#include "EditorContext.h"
#include "Panels/RendererPanel.h"

class EditorUI
{
public:
	void Draw(EditorContext& context);
private:
	RendererPanel m_RendererPanel;
};