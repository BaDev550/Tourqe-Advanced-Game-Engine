#include "GameLayer.h"
#include "imgui.h"
#include "TAGE/Application/Application.h"
#include "TAGE/World/Scene/SceneSerializer.h"

namespace TAGE {
	GameLayer::GameLayer()
	{
		_ActiveScene = MEM::MakeRef<Scene>("SCENE");
		_ActiveScene->Clear();
		SceneSerializer serializer(_ActiveScene);
		if (serializer.Deserialize("Assets/scenes/TPSMario.tage"))
		{
			_ActiveScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
		}
	}

	void GameLayer::OnAttach()
	{
		_ActiveScene->OnRuntimeStart();
	}

	void GameLayer::OnDetach()
	{
	}

	void GameLayer::OnUpdate(float dt)
	{
		_ActiveScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
		_ActiveScene->OnUpdateRuntime(dt);
	}

	void GameLayer::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::Begin("Game Viewport", nullptr, windowFlags);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		auto renderer = Application::Get()->GetRenderer();
		auto deferred = renderer->GetDeferredRendering();
		deferred.GetLightShader()->Use();
		textureID = deferred.GetLightingBuffer()->GetColorAttachment(0);

		ImGui::Image((ImTextureID)(uintptr_t)textureID, ImVec2(_ViewportSize.x, _ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();

		if (Input::IsKeyJustPressed(Key::R)) {
			_ActiveScene->OnRuntimeStop();
			_ActiveScene->Clear();
			SceneSerializer serializer(_ActiveScene);
			if (serializer.Deserialize("Assets/scenes/TPSMario.tage"))
			{
				_ActiveScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
				_ActiveScene->OnRuntimeStart();
			}
		}
	}

	void GameLayer::OnEvent(Event& event)
	{
	}
}