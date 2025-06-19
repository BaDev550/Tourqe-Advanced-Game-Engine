#include "tagepch.h"
#include "Application.h"
#include "TAGE/Utilities/Timer.h"
#include "TAGE/Input/Input.h"

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
		auto _RenderSystem = MEM::MakeRef<System_Renderer>(_Renderer.get());

		_ActiveScene = MEM::MakeRef<Scene>("Test Scene - 1");
		_ActiveScene->AddSystem(_RenderSystem);

		_Camera = MEM::MakeScope<TARE::FreeCamera>();
		Object camera = _ActiveScene->CreateObject("Camera");
		camera.AddComponent<CameraComponent>(_Camera->GetCamera());

		Object mario = _ActiveScene->CreateObject("Mario");
		mario.AddComponent<MeshComponent>("assets/models/mario_2/mario_2.obj");

		Object sponza = _ActiveScene->CreateObject("sponza");
		sponza.AddComponent<MeshComponent>("assets/models/Sponza/Sponza.gltf");
		sponza.GetComponent<TransformComponent>()->Scale = glm::vec3(0.01f, 0.01f, 0.01f);

		Object testModel = _ActiveScene->CreateObject("testModel");
		testModel.AddComponent<MeshComponent>("assets/models/SciFiHelmet/SciFiHelmet.gltf");

		Light sun = Light(LightType::DIRECTIONAL, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		Object sunLight = _ActiveScene->CreateObject("sunLight");
		sunLight.AddComponent<LightComponent>(sun);

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

			_Camera->Update(_DeltaTime);

			for (const auto& layer : _LayerStack)
				layer->OnUpdate(_DeltaTime);

			_ActiveScene->Update(_DeltaTime);

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