﻿#include "tagepch.h"
#include "Application.h"
#include "TAGE/Utilities/Timer.h"
#include "TAGE/Input/Input.h"

#include "TAGE/World/Systems/System_Renderer.h"
#include "TAGE/World/Components/RenderComponents.h"

namespace TAGE {
	Application* Application::s_Instance = nullptr;

	Application::Application(const char* appName, int width, int height)
	{
		s_Instance = this;
		Logger::init();
		LOG_INFO("TAGE Version - {}.{}", ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR);

		ASSERT(s_Instance, "Application already exists");

		const SWindow WINDOW = SWindow(appName, width, height);
		_Window = MEM::MakeScope<Window>(WINDOW, WindowMode::WINDOWED);
		_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		_Window->ToggleCursor(true);
		Input::Init(_Window->GetGLFWWindow());
		width = _Window->GetWidth();
		height = _Window->GetHeight();

		_ThreadPool = MEM::MakeScope<Threading::ThreadPool>(4);

		_ImGuiLayer = MEM::MakeScope<ImGuiLayer>();
		PushOverlay(_ImGuiLayer.get());
	}

	Application::~Application()
	{
		for (Layer* layer : _LayerStack)
			layer->OnDetach();
		_LayerStack.Clear();
		Logger::Destroy();

		LOG_WARN("Application terminated!");
		LOG_WARN("Exiting...");
	}

	void Application::Run()
	{
		Timer timer;

		while (!_Window->ShouldClose() && _ApplicationState == ApplicationState::RUNNING) {
			float time = timer.Elapsed();
			_DeltaTime = time - _LastFrame;
			_LastFrame = time;

			for (const auto& layer : _LayerStack)
				layer->OnUpdate(_DeltaTime);

			_ImGuiLayer->Begin();
			for (const auto& layer : _LayerStack)
				layer->OnImGuiRender();
			_ImGuiLayer->End();

			Input::Update();
			_Window->SwapBuffers();
		}
	}

	void Application::OnEvent(Event& event)
	{
		for (auto it = _LayerStack.end(); it != _LayerStack.begin();) {
			(*--it)->OnEvent(event);
			if (event.bIsHandled)
				break;
		}
	}
}