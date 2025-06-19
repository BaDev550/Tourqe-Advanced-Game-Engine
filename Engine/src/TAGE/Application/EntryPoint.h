#pragma once

#include "TAGE/Application/Application.h"
extern TAGE::Application* TAGE::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	auto app = TAGE::CreateApplication({ argc, argv });
	app->Run();
	delete app;
}