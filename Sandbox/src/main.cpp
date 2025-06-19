#include "TAGE/Application/Application.h"
#include "TAGE/Window/Window.h"
#include "TAGE/World/Components/RenderComponents.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "imgui.h"

class SandboxLayer : public TAGE::Layer {
    virtual void OnUpdate(float dt) {}
    virtual void OnImGuiRender() {
        //auto renderer = TAGE::Application::Get()->GetRenderer();
        auto app = TAGE::Application::Get();

        //if (ImGui::CollapsingHeader("Renderer Debug - TARE 0.1")) {
        //    ImGui::Text("FPS: %d", (int)(1.0f / app->GetDeltaTime()));

        //    ImGui::Text("gBuffer - Position");
        //    const auto& gPos = renderer->GetDeferredRenderer()->GetTexture(TAGE::Renderer::DeferredRendering::POS);
        //    ImGui::Image((ImTextureID)(void*)gPos->GetID(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        //    ImGui::Text("gBuffer - Normal");
        //    const auto& gNormal = renderer->GetDeferredRenderer()->GetTexture(TAGE::Renderer::DeferredRendering::NORMAL);
        //    ImGui::Image((ImTextureID)(void*)gNormal->GetID(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        //    ImGui::Text("gBuffer - Albedo");
        //    const auto& gAlbedo = renderer->GetDeferredRenderer()->GetTexture(TAGE::Renderer::DeferredRendering::ALBEDO);
        //    ImGui::Image((ImTextureID)(void*)gAlbedo->GetID(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        //    ImGui::Text("gBuffer - Light");
        //    const auto& gLight = renderer->GetDeferredRenderer()->GetTexture(TAGE::Renderer::DeferredRendering::LIGHT);
        //    ImGui::Image((ImTextureID)(void*)gLight->GetID(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        //    ImGui::Text("gBuffer - Depth Buffer");
        //    const auto& gDepth = renderer->GetDeferredRenderer()->GetTexture(TAGE::Renderer::DeferredRendering::DEPTH);
        //    ImGui::Image((ImTextureID)(void*)gDepth->GetID(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));

        //    ImGui::Text("Shadow Map");
        //    const auto& ShadowMap = renderer->GetShadowPass()->GetShadowMap();
        //    ImGui::Image((ImTextureID)(void*)ShadowMap->GetDepthMap(), ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));
        //}

        auto scene = app->GetScene();
        for (auto& [id, object] : scene->GetObjects()) {
            auto idenityC = object->GetComponent<TAGE::IdentityComponent>();
            if (ImGui::Selectable(idenityC->Name.c_str())) {
                _SelectedObject = object;
            }
        }

        if (_SelectedObject) {
            auto& transformC = _SelectedObject->GetTransform();

            ImGui::DragFloat3("Position", glm::value_ptr(transformC.Position));
            glm::vec3 euler = glm::eulerAngles(transformC.Rotation) * glm::degrees(1.0f);
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler))) {
                transformC.Rotation = glm::quat(glm::radians(euler));
            }
            ImGui::DragFloat3("Scale", glm::value_ptr(transformC.Scale));

            if (_SelectedObject->HasComponent<TAGE::LightComponent>()) {
                auto& Light = _SelectedObject->GetComponent<TAGE::LightComponent>()->Handle;
                ImGui::DragFloat3("Light Pos", glm::value_ptr(Light.position));
                ImGui::DragFloat3("Light Direction", glm::value_ptr(Light.direction));
                ImGui::ColorEdit3("Light Color", glm::value_ptr(Light.color));
                ImGui::DragFloat("Light inte", &Light.intensity);
            }
        }
    }

    virtual void OnEvent(TAGE::Event& event) {
        
    }
private:
    TAGE::MEM::Ref<TAGE::Object> _SelectedObject;
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