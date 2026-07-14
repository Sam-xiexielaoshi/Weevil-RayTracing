#pragma once
#include "../EditorContext.h"

class ScenePanel
{
public:
	void Draw(EditorContext& context);
private:
	void DrawSpheres(EditorContext& context);
};