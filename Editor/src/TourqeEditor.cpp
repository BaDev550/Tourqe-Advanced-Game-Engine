#include <TAGE/Application/Application.h>
#include <TAGE/Application/EntryPoint.h>

#include "EditorLayer.h"

namespace TAGE::Editor {
	class TourqeEditor : public TAGE::Application
	{
	public:
		TourqeEditor(const TAGE::ApplicationSpecifics& spec)
			: Application(spec)
		{
			PushLayer(new EditorLayer());
		}
	};
}

namespace TAGE {
	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecifics spec;
		spec.AppName = "Tourqe Advanced Game Engine";
		spec.windowSettings.Width = 1280;
		spec.windowSettings.Height = 720;
		spec.windowSettings.VSync = false;
		spec.CommandLineArgs = args;

		return new TAGE::Editor::TourqeEditor(spec);
	}
}