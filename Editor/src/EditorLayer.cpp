#include "EditorLayer.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_internal.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TAGE/World/Scene/SceneSerializer.h"
#include "TAGE/Utilities/Platform.h"

namespace TourqeEditor {
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		_ActiveScene = TAGE::MEM::MakeRef<TAGE::Scene>("Test Scene - 1");
		_EditorCamera = TAGE::MEM::MakeRef<TARE::EditorCamera>(_ActiveScene->GetWidth(), _ActiveScene->GetHeight());
		_SceneHierarchyPanel = TAGE::MEM::MakeScope<SceneHierarchyPanel>(_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(float dt)
	{
		auto[mx, my] = ImGui::GetMousePos();
		mx -= _ViewportBounds[0].x;
		my -= _ViewportBounds[0].y;
		glm::vec2 viewportSize = _ViewportBounds[1] - _ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		auto renderer = TAGE::Application::Get()->GetRenderer();
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
			int pixelData = renderer->GetDeferredRendering().GetLightingBuffer()->Read(1, mouseX, mouseY);
			LOG_WARN(pixelData);
		}

		auto spec = renderer->GetDeferredRendering().GetLightingBuffer()->GetSpecification();
		if (_ViewportSize.x > 0.0 && _ViewportSize.y > 0.0 
			&& (spec.Width != _ViewportSize.x || spec.Height != _ViewportSize.y)) 
		{
			renderer->Resize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
			_ActiveScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
			_EditorCamera->OnResize(_ViewportSize.x, _ViewportSize.y);
			if (_ActiveScene->GetPrimaryCamera()) {
				_ActiveScene->GetPrimaryCamera().GetComponent<TAGE::CameraComponent>().Handle->OnResize(_ViewportSize.x, _ViewportSize.y);
			}
			_LastViewportSize = _ViewportSize;
		}

		if (_ViewportMouseFocused && !ImGuizmo::IsUsing()) {
			_EditorCamera->OnUpdate(dt);
		}
		else {
			_EditorCamera->SetFirstMouse(true);
		}

		_ActiveScene->OnUpdateEditor(dt, _EditorCamera);
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Exit")) TAGE::Application::Get()->Close();
				
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		_SceneHierarchyPanel->OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 pos = ImGui::GetWindowPos();
		auto viewportOffset = ImGui::GetWindowPos();
		_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		_ViewportFocused = ImGui::IsWindowFocused();
		_ViewportHovered = ImGui::IsWindowHovered();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		
		auto renderer = TAGE::Application::Get()->GetRenderer();
		auto deferred = renderer->GetDeferredRendering();
		deferred.GetLightShader()->Use();
		uint64 textureID = 0;

		switch (debugMode)
		{
		case ViewportDebugMode::Default:
			textureID = deferred.GetLightingBuffer()->GetColorAttachment(0);
			break;
		case ViewportDebugMode::GBuffer_Position:
			textureID = deferred.GetGBuffer()->GetColorAttachment(0);
			break;
		case ViewportDebugMode::GBuffer_Normal:
			textureID = deferred.GetGBuffer()->GetColorAttachment(1);
			break;
		case ViewportDebugMode::GBuffer_Albedo:
			textureID = deferred.GetGBuffer()->GetColorAttachment(2);
			break;
		case ViewportDebugMode::GBuffer_Depth:
			textureID = deferred.GetGBuffer()->GetDepthAttachment();
			break;
		case ViewportDebugMode::Lighting:
			textureID = deferred.GetLightingBuffer()->GetColorAttachment(0);
			break;
		case ViewportDebugMode::ShadowMap:
			textureID = renderer->GetShadowMap().GetTextureID();
			break;
		case ViewportDebugMode::GI:
			textureID = deferred.GetGIBuffer()->GetColorAttachment(0);
			break;
		}

		ImGui::Image((ImTextureID)(void*)(uintptr_t)textureID, ImVec2{ _ViewportSize.x, _ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (TAGE::Input::IsKeyJustPressed(E_KEY_E)) _GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		else if (TAGE::Input::IsKeyJustPressed(E_KEY_R)) _GizmoType = ImGuizmo::OPERATION::ROTATE;
		else if (TAGE::Input::IsKeyJustPressed(E_KEY_Q)) _GizmoType = ImGuizmo::OPERATION::SCALE;

		if (_ViewportHovered && TAGE::Input::IsMouseButtonPressed(E_MOUSE_BUTTON_RIGHT)) {
			TAGE::Application::Get()->GetWindow()->ToggleCursor(false);
			_ViewportMouseFocused = true;
		}
		else {
			TAGE::Application::Get()->GetWindow()->ToggleCursor(true);
			_ViewportMouseFocused = false;
		}

		TAGE::Entity* selectedEntity = _SceneHierarchyPanel->GetSelectedEntity();
		if (selectedEntity && _GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);

			auto rect = ImGui::GetWindowSize();
			auto pos = ImGui::GetWindowPos();
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(pos.x, pos.y, rect.x, rect.y);

			//auto cameraEntity = _ActiveScene->GetPrimaryCamera();
			//const auto& camera = cameraEntity.GetComponent<TAGE::CameraComponent>().Handle;

			//const glm::mat4& cameraProj = camera->GetProjectionMatrix();
			//glm::mat4 cameraView = camera->GetViewMatrix();

			const glm::mat4& cameraProj = _EditorCamera->GetProjectionMatrix();
			glm::mat4 cameraView = _EditorCamera->GetViewMatrix();

			auto& tc = selectedEntity->GetComponent<TAGE::TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			bool snap = TAGE::Input::IsKeyPressed(E_KEY_LEFT_SHIFT);
			float snapValue = (_GizmoType == ImGuizmo::OPERATION::ROTATE) ? 45.0f : 0.5f;
			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProj),
				(ImGuizmo::OPERATION)_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotationEuler, scale;
				ImGuizmo::DecomposeMatrixToComponents(
					glm::value_ptr(transform),
					glm::value_ptr(translation),
					glm::value_ptr(rotationEuler),
					glm::value_ptr(scale));

				tc.Position = translation;
				tc.Scale = scale;
				tc.Rotation = glm::quat(glm::radians(rotationEuler));
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}
	void EditorLayer::OnEvent(TAGE::Event& event)
	{
		TAGE::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<TAGE::KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(TAGE::KeyPressedEvent& e)
	{
		bool control = TAGE::Input::IsKeyPressed(E_KEY_LEFT_CONTROL) || TAGE::Input::IsKeyPressed(E_KEY_RIGHT_CONTROL);
		bool shift = TAGE::Input::IsKeyPressed(E_KEY_LEFT_SHIFT) || TAGE::Input::IsKeyPressed(E_KEY_RIGHT_SHIFT);

		switch (e.GetKey())
		{
		case E_KEY_F1: debugMode = ViewportDebugMode::Default; break;
		case E_KEY_F2: debugMode = ViewportDebugMode::GBuffer_Position; break;
		case E_KEY_F3: debugMode = ViewportDebugMode::GBuffer_Normal; break;
		case E_KEY_F4: debugMode = ViewportDebugMode::GBuffer_Albedo; break;
		case E_KEY_F5: debugMode = ViewportDebugMode::GBuffer_Depth; break;
		case E_KEY_F6: debugMode = ViewportDebugMode::Lighting; break;
		case E_KEY_F7: debugMode = ViewportDebugMode::ShadowMap; break;
		case E_KEY_F8: debugMode = ViewportDebugMode::GI; break;

		case E_KEY_S: {
			if (control && shift)
				SaveSceneAs();
			break;
		}
		case E_KEY_N: {
			if (control)
				NewScene();
			break;
		}
		case E_KEY_O: {
			if (control)
				OpenScene();
			break;
		}

		default:
			break;
		}

		if (e.GetKey() == E_KEY_ESCAPE)
		{
			TAGE::Application::Get()->Close();
			return true;
		}
		return true;
	}

	void EditorLayer::NewScene()
	{
		_ActiveScene = TAGE::MEM::MakeRef<TAGE::Scene>("New Scene");
		_ActiveScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
		_SceneHierarchyPanel->SetContext(_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = TAGE::Platform::FileDialog::OpenFile("TAGE Scene (*.tage)\0*.tage\0");
		if (!filepath.empty()) {
			_ActiveScene = TAGE::MEM::MakeRef<TAGE::Scene>("New Scene");
			_ActiveScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
			_SceneHierarchyPanel->SetContext(_ActiveScene);

			TAGE::SceneSerializer serializer(_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = TAGE::Platform::FileDialog::SaveFile("TAGE Scene (*.tage)\0*.tage\0");
		if (!filepath.empty()) {
			TAGE::SceneSerializer serializer(_ActiveScene);
			serializer.Serialize(filepath);
		}
	}
}
