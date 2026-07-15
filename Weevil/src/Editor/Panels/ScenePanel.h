#pragma once
#include "../EditorContext.h"

class ScenePanel
{
public:
	void Draw(EditorContext& context);
private:
	void DrawSpheres(EditorContext& context);
	void DrawMaterials(EditorContext& context);
	void DrawEnvironment(EditorContext& context);
	bool DrawSelectableSphere(const char* label, bool selected);
};