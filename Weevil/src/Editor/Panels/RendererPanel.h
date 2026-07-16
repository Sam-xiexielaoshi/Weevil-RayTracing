#pragma once
#include "../EditorContext.h"
#include "../Core/Panel.h"

class RendererPanel : public Panel
{
public:
	void Draw(EditorContext& context) override;
};