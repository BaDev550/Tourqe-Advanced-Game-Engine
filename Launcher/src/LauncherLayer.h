#pragma once

#include "TAGE/TAGE.h"

namespace TAGE::Launcher {
	class LauncherLayer : public Layer
	{
	public:
		LauncherLayer();
		virtual ~LauncherLayer() = default;
		
		virtual void OnImGuiRender() override;

		void DrawProjectsPanel();
		void DrawCreatePanel();
		void DrawSidebar();
		void DrawMainPanel();
	private:
		void ShowTemplate(int id, const std::string& name, const std::string& description, const std::string& iconPath);

		int _SelectedTemplate = 0;
	};

}
