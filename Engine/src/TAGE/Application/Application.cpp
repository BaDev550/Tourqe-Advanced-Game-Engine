#include "tagepch.h"
#include "Application.h"
#include "TAGE/Utilities/Timer.h"
#include "TAGE/Input/Input.h"
#include "TAGE/Scripting/ScriptEngine.h"

#include "TAGE/World/Systems/System_Renderer.h"
#include "TAGE/World/Components/RenderComponents.h"

namespace TAGE {
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecifics& spec) : _AppSpecifics(spec)
	{
		s_Instance = this;
		Logger::init();
		LOG_INFO("TAGE Version - {}.{}", ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR);

		ASSERT(s_Instance, "Application already exists");
		int width = 1280;
		int height = 720;

		const SWindow WINDOW = SWindow(_AppSpecifics.AppName.c_str(), width, height);
		_Window = MEM::MakeScope<Window>(WINDOW, WindowMode::WINDOWED);
		_Renderer = MEM::MakeScope<TARE::TARE>(width, height);
		_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		_Window->ToggleCursor(true);

		Input::Init(_Window->GetGLFWWindow());
		ScriptEngine::Init();

		_ImGuiLayer = MEM::MakeScope<ImGuiLayer>();
		PushOverlay(_ImGuiLayer.get());
	}

	Application::~Application()
	{
		for (Layer* layer : _LayerStack)
			layer->OnDetach();
		_LayerStack.Clear();
		ScriptEngine::Shutdown();

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
		for (auto it = _LayerStack.rbegin(); it != _LayerStack.rend(); ++it)
		{
			//if (event.bIsHandled)
			//	break;
			(*it)->OnEvent(event);
		}
	}
}