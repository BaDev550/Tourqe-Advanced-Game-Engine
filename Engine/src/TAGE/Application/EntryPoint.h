#pragma once

#include "TAGE/Application/Application.h"
#include "TAGE/Utilities/Profiler.h"
extern TAGE::Application* TAGE::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	PROFILE_BEGIN_SESSION("Startup", "TAGEProfile-Startup.json");
	auto app = TAGE::CreateApplication({ argc, argv });
	PROFILE_END_SESSION();

	PROFILE_BEGIN_SESSION("Runtime", "TAGEProfile-Runtime.json");
	app->Run();
	PROFILE_END_SESSION();

	PROFILE_BEGIN_SESSION("Shutdown", "TAGEProfile-Shutdown.json");
	delete app;
	PROFILE_END_SESSION();
}