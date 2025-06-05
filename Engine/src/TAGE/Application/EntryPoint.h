#pragma once

#include "TAGE/Application/Application.h"
extern TAGE::Application* TAGE::CreateApplication();

int main(int argc, char** argv)
{
	auto app = TAGE::CreateApplication();
	app->Run();
	delete app;
}