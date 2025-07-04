#pragma once

#include "TAGE/Window/Window.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Utilities/Logger.h"
#include "TAGE/Layers/LayerStack.h"
#include "TAGE/Layers/ImGuiLayer.h"

#include "TAGE/Events/Event.h"
#include "TAGE/Events/ApplicationEvents.h"

#include "TAGE/Thread/ThreadWarpper.h"
#include "TAGE/AssetManager/AssetManager.h"
#include "TAGE/World/Scene/Scene.h"

#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/MultiThread/MultiThread.h"

#include "TARE/TARE.h"

namespace TAGE {
    struct ApplicationCommandLineArgs {
        int Count = 0;
        char** Args = nullptr;

        const char* operator[](int index) const
        {
            ASSERT_NOMSG(index < Count);
            return Args[index];
        }
    };

	struct ApplicationSpecifics
	{
		std::string AppName = "TAGE";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

    class Application
    {
    public:
        Application(const ApplicationSpecifics& spec);
        virtual ~Application();
        void Run();

        static Application* Get() { return s_Instance; }
        const float GetDeltaTime() const { return _DeltaTime; }
        ApplicationState GetApplicationState() const { return _ApplicationState; }
        ApplicationSpecifics GetSpecification() const { return _AppSpecifics; }

        void PushLayer(Layer* layer) { _LayerStack.PushLayer(layer); }
        void PushOverlay(Layer* layer) { _LayerStack.PushOverlay(layer); }

		void Close() {
			_ApplicationState = ApplicationState::STOPPED;
			_Window->ForceClose();
		}

    public:
		TARE::TARE* GetRenderer() const { return _Renderer.get(); }
        Window* GetWindow() const { return _Window.get(); }
		ImGuiLayer* GetImGuiLayer() const { return _ImGuiLayer.get(); }
        ThreadPool& GetThreadPool() { return _ThreadPool; }
        GrapichDispatcher& GetGraphicDispatcher() { return _GraphicDispatcher; };
    private:
        void OnEvent(Event& event);
    private:
        static Application* s_Instance;
        ApplicationState _ApplicationState = ApplicationState::RUNNING;
		ApplicationSpecifics _AppSpecifics;
        ThreadPool _ThreadPool;
        GrapichDispatcher _GraphicDispatcher;

        MEM::Scope<Window> _Window;
        MEM::Scope<TARE::TARE> _Renderer;
        MEM::Scope<ImGuiLayer> _ImGuiLayer;
        LayerStack _LayerStack;

        float _LastFrame = 0.0f;
        float _DeltaTime = 0.0f;
    };

    Application* CreateApplication(ApplicationCommandLineArgs args);
}
