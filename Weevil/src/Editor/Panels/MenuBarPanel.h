#pragma once

#include "../EditorContext.h"
#include "../Core/Panel.h"

class MenuBarPanel : public Panel
{
public:
	void Draw(EditorContext& context) override;
};