#include <TAGE/Application/Application.h>
#include <TAGE/Application/EntryPoint.h>

#include "LauncherLayer.h"

namespace TAGE::Launcher {
	class TourqeEditor : public TAGE::Application
	{
	public:
		TourqeEditor(const TAGE::ApplicationSpecifics& spec)
			: Application(spec)
		{
			PushLayer(new LauncherLayer());
		}
	};
}

namespace TAGE {
	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecifics spec;
		spec.windowSettings.Width = 900;
		spec.windowSettings.Height = 900;
		spec.windowSettings.Resizable = false;
		spec.UseTARE = false;
		spec.AppName = "TAGE Launcher";
		spec.CommandLineArgs = args;

		return new TAGE::Launcher::TourqeEditor(spec);
	}
}