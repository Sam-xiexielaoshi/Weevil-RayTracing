#include "RendererPanel.h"

#include <imgui.h>

void RendererPanel::Draw(EditorContext& context)
{
	Renderer& renderer = *context.Renderer;

	ImGui::Begin("Settings");
	ImGui::Text("Last render time: %.3fms", *context.LastRenderTime);
	//accumulate checkbox ui
	ImGui::Checkbox("Accumulate", &renderer.GetSettings().Accumulate);
	ImGui::Separator();

	//Tone MApper settings ui
	if (ImGui::TreeNodeEx("Tone Mapping", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool toneMappingChanged = false;
		const char* toneMappers[]
		{
			"None",
			"Reinhard",
			"ACES Filmic",
			"Hable"
		};
		int currentToneMapper = static_cast<int>(renderer.GetSettings().ToneMapping);
		toneMappingChanged |= ImGui::DragFloat(
			"Exposure",
			&renderer.GetSettings().Exposure,
			0.05f,
			0.0f,
			10.0f);
		toneMappingChanged |= ImGui::Combo(
			"Tone Mapper",
			&currentToneMapper,
			toneMappers,
			IM_ARRAYSIZE(toneMappers));
		if (toneMappingChanged)
		{
			renderer.GetSettings().ToneMapping =
				static_cast<Renderer::ToneMapper>(currentToneMapper);
		}
		toneMappingChanged |= ImGui::DragFloat(
			"Gamma",
			&renderer.GetSettings().Gamma,
			0.05f,
			0.0f,
			5.0f);
		if (toneMappingChanged)
			renderer.ResetFrameIndex();
		ImGui::TreePop();
	}
	ImGui::Separator();

	//bloom settings ui
	if (ImGui::TreeNodeEx("Bloom", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char* bloomFilters[]
		{
			"Box",
			"Gaussian"
		};
		int currentBloomFilter = static_cast<int>(renderer.GetSettings().BloomFilter);
		bool bloomChanged = false;
		bloomChanged |= ImGui::Combo(
			"Filter",
			&currentBloomFilter,
			bloomFilters,
			IM_ARRAYSIZE(bloomFilters));
		if (bloomChanged)
			renderer.GetSettings().BloomFilter = static_cast<Renderer::BloomFilter>(currentBloomFilter);
		bloomChanged |= ImGui::DragFloat(
			"Threshold",
			&renderer.GetSettings().BloomThreshold,
			0.05f,
			0.0f,
			10.0f);
		bloomChanged |= ImGui::DragInt(
			"Radius",
			&renderer.GetSettings().BloomRadius,
			1,
			1,
			20);
		bloomChanged |= ImGui::DragFloat(
			"Strength",
			&renderer.GetSettings().BloomStrength,
			0.05f,
			0.0f,
			5.0f);

		if (bloomChanged)
		{
			renderer.GenerateGaussianKernel();
			renderer.ResetFrameIndex();
		}
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNodeEx("Path Tracing", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool changed = false;
		changed |= ImGui::Checkbox(
			"Russian Roulette",
			&renderer.GetSettings().EnableRussianRoulette);
		changed |= ImGui::DragInt(
			"Max Bounces",
			&renderer.GetSettings().MaxBounces,
			1,
			3,
			20);
		if (changed)
			renderer.ResetFrameIndex();
		ImGui::TreePop();
	}
	ImGui::Separator();

	//reset button
	if (ImGui::Button("Reset"))
		renderer.ResetFrameIndex();

	ImGui::Text("Frame Index: %u", renderer.GetFinalIndex());

	ImGui::End();
}