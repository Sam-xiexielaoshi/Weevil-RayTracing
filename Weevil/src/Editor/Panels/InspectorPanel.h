#pragma once

#include "../EditorContext.h"

class InspectorPanel
{
public:
	void Draw(EditorContext& context);
private:
	void DrawSphere(EditorContext& context);
	void DrawMaterial(EditorContext& context);
	void DrawEnvironment(EditorContext& context);
};