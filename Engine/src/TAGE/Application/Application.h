#pragma once

#include "TAGE/Window/Window.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Utilities/Logger.h"
#include "TAGE/Layers/LayerStack.h"
#include "TAGE/Layers/ImGuiLayer.h"

#include "TAGE/Events/Event.h"
#include "TAGE/Events/ApplicationEvents.h"

#include "TAGE/Renderer/Renderer.h"
#include "TAGE/Renderer/Camera/FreeCamera.h"
#include "TAGE/Renderer/Model/Model.h"

#include "TAGE/Thread/ThreadWarpper.h"
#include "TAGE/Renderer/Light/LightManager.h"

namespace TAGE {
    using namespace Renderer;
    class Application
    {
    public:
        Application(const char* appName, int width, int height);
        virtual ~Application();
        void Run();

        static Application* Get() { return s_Instance; }

        void PushLayer(Layer* layer) { _LayerStack.PushLayer(layer); }
        void PushOverlay(Layer* layer) { _LayerStack.PushOverlay(layer); }
    public:
        Window* GetWindow() const { return _Window.get(); }
        Renderer::GL_Renderer* GetRenderer() const { return _Renderer.get(); }
        Threading::ThreadPool* GetThreadPool() const { return _ThreadPool.get(); }
        ApplicationState& GetApplicationState() { return _ApplicationState; }
        LightManager& GetLightManager() { return lightManager; }
    private:
        void OnEvent(Event& event);
    private:
        static Application* s_Instance;
        ApplicationState _ApplicationState = ApplicationState::RUNNING;

        MEM::Scope<Window> _Window;
        MEM::Scope<GL_Renderer> _Renderer;
        MEM::Scope<ImGuiLayer> _ImGuiLayer;
        MEM::Scope<FreeCamera> _FreeCam;
        MEM::Scope<Threading::ThreadPool> _ThreadPool;
        LayerStack _LayerStack;

        Model _Test;
        Model _Floor;
        LightManager lightManager;

        float _LastFrame = 0.0f;
        float _DeltaTime = 0.0f;
    };

    Application* CreateApplication();
}
