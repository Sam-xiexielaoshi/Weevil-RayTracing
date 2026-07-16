#include "InspectorPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void InspectorPanel::Draw(EditorContext& context)
{
    ImGui::Begin("Inspector");

    switch (context.SelectedType)
    {
    case SelectionType::Sphere:
        DrawSphere(context);
        break;

    case SelectionType::Material:
        DrawMaterial(context);
        break;

    case SelectionType::Environment:
        DrawEnvironment(context);
        break;

    default:

        ImGui::TextDisabled("Nothing Selected");

        ImGui::Separator();

        ImGui::TextWrapped(
            "Select an object from the Scene panel.");

        break;
    }

    ImGui::End();
}

void InspectorPanel::DrawSphere(EditorContext& context)
{
    Scene& scene = *context.Scene;
    Renderer& renderer = *context.Renderer;

    if (context.SelectedIndex < 0 ||
        context.SelectedIndex >= (int)scene.Spheres.size())
        return;

    Sphere& sphere = scene.Spheres[context.SelectedIndex];

    ImGui::Text("Sphere %d", context.SelectedIndex);

    ImGui::Separator();

    bool changed = false;

    changed |= ImGui::DragFloat3(
        "Position",
        glm::value_ptr(sphere.Position),
        0.1f);

    changed |= ImGui::DragFloat(
        "Radius",
        &sphere.Radius,
        0.1f);

    changed |= ImGui::DragInt(
        "Material",
        &sphere.MaterialIndex,
        1,
        0,
        (int)scene.Materials.size() - 1);

    if (changed)
        renderer.ResetFrameIndex();
}

void InspectorPanel::DrawMaterial(EditorContext& context)
{
    ImGui::Text("Material %d", context.SelectedIndex);
}

void InspectorPanel::DrawEnvironment(EditorContext& context)
{
    ImGui::Text("Environment");
}