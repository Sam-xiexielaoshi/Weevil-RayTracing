#include "EditorCommands.h"

void EditorCommands::NotifySceneChanged(EditorContext& context)
{
	context.Renderer->ResetFrameIndex();
}

void EditorCommands::CreateSphere(EditorContext& context)
{
	Scene& scene = *context.Scene;

	Sphere sphere;
	sphere.Position = { 0.0f, 0.0f, 0.0f };
	sphere.Radius = 0.5f;
	sphere.MaterialIndex = 0;

	scene.Spheres.push_back(sphere);

	context.SelectedType = SelectionType::Sphere;
	context.SelectedIndex = (int)scene.Spheres.size() - 1;

	NotifySceneChanged(context);
}

void EditorCommands::DeleteSphere(EditorContext& context, int index)
{
	Scene& scene = *context.Scene;

	if (index < 0 || index >= (int)scene.Spheres.size())
		return;

	scene.Spheres.erase(scene.Spheres.begin() + index);

	context.SelectedType = SelectionType::None;
	context.SelectedIndex = -1;

	NotifySceneChanged(context);
}

void EditorCommands::CreateMaterial(EditorContext& context)
{
	Scene& scene = *context.Scene;

	Material material;

	scene.Materials.push_back(material);

	context.SelectedType = SelectionType::Material;
	context.SelectedIndex = (int)scene.Materials.size() - 1;

	NotifySceneChanged(context);
}

void EditorCommands::DeleteMaterial(EditorContext& context, int index)
{
	Scene& scene = *context.Scene;

	if (index < 0 || index >= (int)scene.Materials.size())
		return;

	scene.Materials.erase(scene.Materials.begin() + index);

	context.SelectedType = SelectionType::None;
	context.SelectedIndex = -1;

	NotifySceneChanged(context);
}