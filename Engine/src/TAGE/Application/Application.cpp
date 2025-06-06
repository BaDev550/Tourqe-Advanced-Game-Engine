#include "tagepch.h"
#include "Application.h"
#include "TAGE/Utilities/Timer.h"
#include "TAGE/Input/Input.h"

namespace TAGE {
	Application* Application::s_Instance = nullptr;

	Application::Application(const char* appName, int width, int height)
	{
		s_Instance = this;
		Logger::init();

		ASSERT(s_Instance, "Application already exists");

		const SWindow WINDOW = SWindow(appName, width, height);
		_Window = MEM::MakeScope<Window>(WINDOW, WindowMode::WINDOWED);
		_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		_Window->ToggleCursor(true);
		Input::Init(_Window->GetGLFWWindow());
		width = _Window->GetWidth();
		height = _Window->GetHeight();

		_Renderer = MEM::MakeScope<GL_Renderer>(width, height);
		_FreeCam = MEM::MakeScope<FreeCamera>();

		_ThreadPool = MEM::MakeScope<Threading::ThreadPool>(4);
		_Test.LoadFromFile("models/weapons/AR/scene.gltf");

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

			_FreeCam->Update(_DeltaTime);

			for (const auto& layer : _LayerStack)
				layer->OnUpdate(_DeltaTime);

			_Renderer->BeginGBuffer(_FreeCam->GetCamera());
			_Test.SetTransform(glm::mat4(1.0f));
			_Test.Draw("DeferredShader");
			_Renderer->BeginFrame(_FreeCam->GetCamera(), {});
			_Test.SetTransform(glm::mat4(1.0f));
			_Test.Draw("MainShader");
			_Renderer->EndFrame();

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