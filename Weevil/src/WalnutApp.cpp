#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:

	ExampleLayer() : m_Camera(60.0f, 0.1f, 100.0f) 
	{
		Material& pinkSphere = m_Scene.Materials.emplace_back();
		pinkSphere.Albedo = { 1.0f, 0.0f, 1.0f };
		pinkSphere.Roughness = 0.0f;

		Material& blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
		blueSphere.Roughness = 0.1f;

		Material& orangeSphere = m_Scene.Materials.emplace_back();
		orangeSphere.Albedo = { 0.8f, 0.5f, 0.2f };
		orangeSphere.Roughness = 0.1f;
		orangeSphere.EmissionColor = orangeSphere.Albedo;
		orangeSphere.EmissionPower = 2.0f;

		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 2.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}
	}

	virtual void OnUpdate(float ts) override
	{
		if(m_Camera.OnUpdate(ts))
			m_Renderer.ResertFrameIndex();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render time: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		
		//accumate checkbox ui
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumate);
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
			int currentToneMapper = static_cast<int>(m_Renderer.GetSettings().ToneMapping);
			toneMappingChanged |= ImGui::DragFloat(
				"Exposure",
				&m_Renderer.GetSettings().Exposure,
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
				m_Renderer.GetSettings().ToneMapping =
					static_cast<Renderer::ToneMapper>(currentToneMapper);
			}
			toneMappingChanged |= ImGui::DragFloat(
				"Gamma",
				&m_Renderer.GetSettings().Gamma,
				0.05f,
				0.0f,
				5.0f);
			if (toneMappingChanged)
				m_Renderer.ResertFrameIndex();
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
			int currentBloomFilter = static_cast<int>(m_Renderer.GetSettings().BloomFilter);
			bool bloomChanged = false;
			bloomChanged |= ImGui::Combo(
				"Filter",
				&currentBloomFilter,
				bloomFilters,
				IM_ARRAYSIZE(bloomFilters));
			if(bloomChanged)
				m_Renderer.GetSettings().BloomFilter = static_cast<Renderer::BloomFilter>(currentBloomFilter);
			bloomChanged |= ImGui::DragFloat(
				"Threshold",
				&m_Renderer.GetSettings().BloomThreshold,
				0.05f,
				0.0f,
				10.0f);
			bloomChanged |= ImGui::DragInt(
				"Radius",
				&m_Renderer.GetSettings().BloomRadius,
				1,
				1,
				20);
			bloomChanged |= ImGui::DragFloat(
				"Strength",
				&m_Renderer.GetSettings().BloomStrength,
				0.05f,
				0.0f,
				5.0f);

			if (bloomChanged)
			{
				m_Renderer.GenerateGaussianKernal();
				m_Renderer.ResertFrameIndex();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();

		if (ImGui::TreeNodeEx("Path Tracing", ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool changed = false;
			changed |= ImGui::Checkbox(
				"Russian Roulette",
				&m_Renderer.GetSettings().EnableRussianRoulette);
			changed |= ImGui::DragInt(
				"Max Bounces",
				&m_Renderer.GetSettings().MaxBounces,
				1,
				3,
				20);
			if (changed)
				m_Renderer.ResertFrameIndex();
			ImGui::TreePop();
		}
		ImGui::Separator();

		//reset button
		if(ImGui::Button("Reset"))
			m_Renderer.ResertFrameIndex();

		ImGui::Text("Frame Index: %u", m_Renderer.GetFinalIndex());

		ImGui::End();

		ImGui::Begin("Scene");

		//spheres tree node
		if (ImGui::TreeNodeEx("Spheres", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
			{
				ImGui::PushID(i);
				std::string label = "Sphere " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					Sphere& sphere = m_Scene.Spheres[i];
					bool sphereChanged = false;
					sphereChanged |= ImGui::DragFloat3(
						"Position",
						glm::value_ptr(sphere.Position),
						0.1f);
					sphereChanged |= ImGui::DragFloat(
						"Radius",
						&sphere.Radius,
						0.1f);
					sphereChanged |= ImGui::DragInt(
						"Material",
						&sphere.MaterialIndex,
						1,
						0,
						(int)m_Scene.Materials.size() - 1);
					if (sphereChanged)
						m_Renderer.ResertFrameIndex();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();

		//materials tree node
		if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (size_t i = 0; i < m_Scene.Materials.size(); i++)
			{
				ImGui::PushID(i);
				std::string label = "Material " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					Material& material = m_Scene.Materials[i];
					bool materialChanged = false;
					// Material Type
					const char* materialTypes[]
					{
						"Diffuse",
						"Metal",
						"Glass"
					};
					int currentType = static_cast<int>(material.Type);
					if (ImGui::Combo(
						"Material Type",
						&currentType,
						materialTypes,
						IM_ARRAYSIZE(materialTypes)))
					{
						material.Type = static_cast<MaterialType>(currentType);
						// Reset properties that don't belong
						switch (material.Type)
						{
						case MaterialType::Diffuse:
							material.Metallic = 0.0f;
							material.RefractionIndex = 1.52f;
							break;

						case MaterialType::Metal:
							material.RefractionIndex = 1.52f;
							break;

						case MaterialType::Dielectric:
							material.Metallic = 0.0f;
							break;
						}
						materialChanged = true;
					}
					// Surface
					materialChanged |= ImGui::ColorEdit3(
						"Albedo",
						glm::value_ptr(material.Albedo));

					materialChanged |= ImGui::DragFloat(
						"Roughness",
						&material.Roughness,
						0.05f,
						0.0f,
						1.0f);

					if (material.Type == MaterialType::Metal)
					{
						materialChanged |= ImGui::DragFloat(
							"Metallic",
							&material.Metallic,
							0.05f,
							0.0f,
							1.0f);
					}

					if (material.Type == MaterialType::Dielectric)
					{
						materialChanged |= ImGui::DragFloat(
							"IOR",
							&material.RefractionIndex,
							0.01f,
							1.0f,
							2.5f,
							"%.2f");
					}
					ImGui::Separator();
					// Emission

					materialChanged |= ImGui::ColorEdit3(
						"Emission Color",
						glm::value_ptr(material.EmissionColor));

					materialChanged |= ImGui::DragFloat(
						"Emission Power",
						&material.EmissionPower,
						0.1f,
						0.0f,
						FLT_MAX);

					if (materialChanged)
						m_Renderer.ResertFrameIndex();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Environment", ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool changed = false;

			static float exposure = 1.0f;
			static float rotation = 0.0f;

			changed |= ImGui::DragFloat(
				"Exposure",
				&exposure,
				0.05f,
				0.0f,
				10.0f);

			changed |= ImGui::DragFloat(
				"Rotation",
				&rotation,
				1.0f,
				0.0f,
				360.0f);

			if (changed)
			{
				m_Renderer.GetEnvironment().SetExposure(exposure);
				m_Renderer.GetEnvironment().SetRotation(rotation);

				m_Renderer.ResertFrameIndex();
			}

			ImGui::TreePop();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		
		auto image = m_Renderer.GetFinalImage();
		if(image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0,1), ImVec2(1,0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		//Renderer Resize
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		//Renderer Render
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Weevil RayTracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}