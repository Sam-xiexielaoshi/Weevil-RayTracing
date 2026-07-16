#pragma once

#include "Panel.h"
#include <memory>
#include <vector>

class PanelManager
{
public:
	template<typename T, typename... Args>
	T* AddPanel(Args&&... args)
	{
		auto panel = std::make_unique<T>(std::forward<Args>(args)...);
		T* ptr = panel.get();
		m_Panels.emplace_back(std::move(panel));
		return ptr;
	}
	void Draw(EditorContext& context)
	{
		for (auto& panel : m_Panels)
		{
			panel->Draw(context);
		}
	}
private:
	std::vector<std::unique_ptr<Panel>> m_Panels;
};