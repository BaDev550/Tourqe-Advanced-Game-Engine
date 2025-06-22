#include <TAGE/Application/Application.h>
#include <TAGE/Application/EntryPoint.h>

#include "EditorLayer.h"

namespace TourqeEditor {
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
		spec.AppName = "Tourqe Editor";
		spec.CommandLineArgs = args;

		return new TourqeEditor::TourqeEditor(spec);
	}
}