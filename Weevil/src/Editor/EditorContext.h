#pragma once
//
//    float* LastRenderTime = nullptr;
//
//    uint32_t* ViewportWidth = nullptr;
//    uint32_t* ViewportHeight = nullptr;
//};

#include "../Renderer/Renderer.h"
#include "../Core/Camera.h"
#include "../Scene/Scene.h"

struct EditorContext
{
	Renderer* Renderer = nullptr;
	Camera* Camera = nullptr;
	Scene* Scene = nullptr;
	float* LastRenderTime = nullptr;
	uint32_t* ViewportWidth = nullptr;
	uint32_t* ViewportHeight = nullptr;
};