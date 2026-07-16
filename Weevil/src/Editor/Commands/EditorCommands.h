#pragma once

#include "../EditorContext.h"

class EditorCommands
{
public:
	//Sphere comm
	static void CreateSphere(EditorContext& context);
	static void DeleteSphere(EditorContext& context, int index);

	//material comm
	static void CreateMaterial(EditorContext& context);
	static void DeleteMaterial(EditorContext& context, int index);
	//util
	static void NotifySceneChanged(EditorContext& context);
};