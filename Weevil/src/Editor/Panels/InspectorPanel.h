#pragma once

#include "../EditorContext.h"
#include "../Core/Panel.h"

class InspectorPanel : public Panel
{
public:
	void Draw(EditorContext& context) override;
private:
	void DrawSphere(EditorContext& context);
	void DrawMaterial(EditorContext& context);
	void DrawEnvironment(EditorContext& context);
};