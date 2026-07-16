#pragma once
#include "../EditorContext.h"
#include "../Core/Panel.h"

class ScenePanel : public Panel
{
public:
	void Draw(EditorContext& context) override;
private:
	void DrawSpheres(EditorContext& context);
	void DrawMaterials(EditorContext& context);
	void DrawEnvironment(EditorContext& context);
};