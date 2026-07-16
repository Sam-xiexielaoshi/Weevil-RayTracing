#pragma once

#include "../EditorContext.h"
#include "../Core/Panel.h"

class ViewportPanel : public Panel
{
public:
	void Draw(EditorContext& context) override;
};