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

namespace TAGE {
    class Application
    {
    public:
        Application(const char* appName, int width, int height);
        virtual ~Application();
        void Run();

        static Application* Get() { return s_Instance; }
        const float GetDeltaTime() const { return _DeltaTime; }
        ApplicationState& GetApplicationState() { return _ApplicationState; }

        void PushLayer(Layer* layer) { _LayerStack.PushLayer(layer); }
        void PushOverlay(Layer* layer) { _LayerStack.PushOverlay(layer); }
    public:
        Scene* GetScene() const { return _ActiveScene.get(); }
        Window* GetWindow() const { return _Window.get(); }
        Threading::ThreadPool* GetThreadPool() const { return _ThreadPool.get(); }
    private:
        void OnEvent(Event& event);
    private:
        static Application* s_Instance;
        ApplicationState _ApplicationState = ApplicationState::RUNNING;

        MEM::Scope<Window> _Window;
        MEM::Scope<ImGuiLayer> _ImGuiLayer;
        MEM::Scope<Threading::ThreadPool> _ThreadPool;
        MEM::Scope<Scene> _ActiveScene;
        LayerStack _LayerStack;

        float _LastFrame = 0.0f;
        float _DeltaTime = 0.0f;
    };

    Application* CreateApplication();
}
