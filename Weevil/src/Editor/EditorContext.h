#pragma once

#include "../Renderer/Renderer.h"
#include "../Core/Camera.h"
#include "../Scene/Scene.h"

enum class SelectionType
{
	None = 0,
	Sphere,
	Material,
	Environment,
	Camera,
	Light
};

struct EditorContext
{
	Renderer* Renderer = nullptr;
	Camera* Camera = nullptr;
	Scene* Scene = nullptr;
	float* LastRenderTime = nullptr;
	uint32_t* ViewportWidth = nullptr;
	uint32_t* ViewportHeight = nullptr;
	SelectionType SelectedType = SelectionType::None;
	int SelectedIndex = -1;
};