#include "TAGE/Application/Application.h"
#include "TAGE/Window/Window.h"
#include "imgui.h"

class SandboxLayer : public TAGE::Layer {
    virtual void OnUpdate(float dt) {}
    virtual void OnImGuiRender() {
        ImGui::Text("gBuffer - Position");
        auto renderer = TAGE::Application::Get()->GetRenderer();
        auto app = TAGE::Application::Get();

        const auto& gPos = renderer->GetDeferredRenderer()->GetTexture(TAGE::Renderer::DeferredRendering::POS);
        ImGui::Image((ImTextureID)(void*)gPos->GetID(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("gBuffer - Normal");
        const auto& gNormal = renderer->GetDeferredRenderer()->GetTexture(TAGE::Renderer::DeferredRendering::NORMAL);
        ImGui::Image((ImTextureID)(void*)gNormal->GetID(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("gBuffer - Albedo");
        const auto& gAlbedo = renderer->GetDeferredRenderer()->GetTexture(TAGE::Renderer::DeferredRendering::ALBEDO);
        ImGui::Image((ImTextureID)(void*)gAlbedo->GetID(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("Shadow Map");
        const auto& ShadowMap = renderer->GetShadowPass()->GetShadowMap();
        ImGui::Image((ImTextureID)(void*)ShadowMap->GetDepthMap(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::DragFloat3("Light Pos", glm::value_ptr(app->GetLightManager().GetLights()[0].position));
        ImGui::DragFloat3("Light Direction", glm::value_ptr(app->GetLightManager().GetLights()[0].direction));
        ImGui::DragFloat("Light inte", &app->GetLightManager().GetLights()[0].intensity);
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