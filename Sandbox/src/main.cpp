#include "TAGE/Application/Application.h"
#include "TAGE/Window/Window.h"
#include "imgui.h"

class SandboxLayer : public TAGE::Layer {
    virtual void OnUpdate(float dt) {}
    virtual void OnImGuiRender() {
        ImGui::Text("Test");
    }

    virtual void OnEvent(TAGE::Event& event) {
        
    }
};

class Sandbox : public TAGE::Application {
public:
    Sandbox() : TAGE::Application("TAGE", 1270, 720)
    {
        PushLayer(new SandboxLayer());
    }

    ~Sandbox() override = default;
};

TAGE::Application* CreateApplication() {
    return new Sandbox();
}

int main() {
    auto app = CreateApplication();
    app->Run();
    delete app;
    return 0;
}