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
		_AppSpecifics.windowSettings.Title = spec.AppName.c_str();
		int width = _AppSpecifics.windowSettings.Width;
		int height = _AppSpecifics.windowSettings.Height;
		_Window = MEM::MakeScope<Window>(_AppSpecifics.windowSettings, WindowMode::WINDOWED);

		_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		_Window->ToggleCursor(false);
		Input::Init(_Window->GetGLFWWindow());

		_ImGuiLayer = MEM::MakeScope<ImGuiLayer>();
		PushOverlay(_ImGuiLayer.get());
	}

	void Application::Shutdown() {
		_ApplicationState = ApplicationState::STOPPED;
		_Window->ForceClose();
	}

	Application::~Application()
	{
		for (Layer* layer : _LayerStack)
			layer->OnDetach();
		_LayerStack.Clear();

		LOG_WARN("Application terminated!");
		LOG_WARN("Exiting...");

		if (ScriptEngine::IsInitialized()) {
			ScriptEngine::Shutdown();
			SAVE_LOG_TO_CACHE();
		}
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

			static bool cursor = false;
			if (Input::IsKeyJustPressed(Key::Tab)) {
				_Window->ToggleCursor(cursor);
				cursor = !cursor;
			}

			Input::Update();
			GrapichDispatcher::Get()->ExecutePending();
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