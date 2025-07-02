#include "EditorLayer.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_internal.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TAGE/World/Scene/SceneSerializer.h"
#include "TAGE/Utilities/Platform.h"
#include "TAGE/Common/TMath.h"

namespace TAGE::Editor {
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		_EditorScene = MEM::MakeRef<Scene>("Editor Scene - 1");
		_ActiveScene = _EditorScene;
		_EditorCamera = MEM::MakeRef<TARE::EditorCamera>(_ActiveScene->GetWidth(), _ActiveScene->GetHeight());
		_SceneHierarchyPanel = MEM::MakeScope<SceneHierarchyPanel>(_ActiveScene);
		_ContentBrowserPanel = MEM::MakeScope<ContentBrowserPanel>();
		_OutputPanel = MEM::MakeScope<OutputPanel>();

		_PlayIcon = TARE::Texture2D::Create();
		_PlayIcon->LoadTexture("Assets/textures/Icons/Play.png");
		_StopIcon = TARE::Texture2D::Create();
		_StopIcon->LoadTexture("Assets/textures/Icons/Stop.png");

		ImGui::GetIO().IniFilename = "editor_layout.ini";
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

		auto renderer = Application::Get()->GetRenderer();
		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
			renderer->GetDeferredRendering().GetGBuffer()->Bind();
			_HoveredEntityID = renderer->GetDeferredRendering().GetGBuffer()->Read(3, mouseX, mouseY);
			renderer->GetDeferredRendering().GetGBuffer()->Unbind();

			if (Input::IsMouseButtonJustPressed(Mouse::ButtonLeft) && !ImGuizmo::IsOver()) {
				auto hoveredEntity = _ActiveScene->GetEntityByID((entt::entity)_HoveredEntityID);
				if (hoveredEntity) {
					if (hoveredEntity.HasComponent<MeshComponent>())
						hoveredEntity.GetComponent<MeshComponent>().IsSelected = true;
					_SceneHierarchyPanel->SetSelectedEntity(hoveredEntity);
				}
				else {
					_SceneHierarchyPanel->SetSelectedEntity({});
				}
			}
		}

		auto spec = renderer->GetDeferredRendering().GetLightingBuffer()->GetSpecification();
		if (_ViewportSize.x > 0.0 && _ViewportSize.y > 0.0 
			&& (spec.Width != _ViewportSize.x || spec.Height != _ViewportSize.y)) 
		{
			renderer->Resize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
			_ActiveScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
			_EditorCamera->OnResize(_ViewportSize.x, _ViewportSize.y);
			if (_ActiveScene->GetPrimaryCamera()) {
				_ActiveScene->GetPrimaryCamera().GetComponent<CameraComponent>().Handle->OnResize(_ViewportSize.x, _ViewportSize.y);
			}
			_LastViewportSize = _ViewportSize;
		}

		switch (_SceneState)
		{
		case SceneState::EDIT:
			if (_ViewportMouseFocused && !ImGuizmo::IsUsing()) {
				_EditorCamera->OnUpdate(dt);
			}
			else {
				_EditorCamera->SetFirstMouse(true);
			}

			_ActiveScene->OnUpdateEditor(dt, _EditorCamera);
			break;
		case SceneState::PLAY:
			_ActiveScene->OnUpdateRuntime(dt);
			break;
		case SceneState::SIMULATE:
			if (_ViewportMouseFocused && !ImGuizmo::IsUsing()) {
				_EditorCamera->OnUpdate(dt);
			}
			else {
				_EditorCamera->SetFirstMouse(true);
			}

			_ActiveScene->OnUpdateSimulate(dt, _EditorCamera);
			break;
		default:
			break;
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		UI_BeginDockspace();

		_SceneHierarchyPanel->OnImGuiRender();
		_ContentBrowserPanel->OnImGuiRender();
		_OutputPanel->OnImGuiRender();

		UI_DrawSettingsPanel();
		UI_DrawViewport();

		UI_EndDockspace();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (_SceneState != SceneState::EDIT)
			return;

		Entity selectedE = _SceneHierarchyPanel->GetSelectedEntity();
		if (selectedE) {
			_EditorScene->DuplicateEntity(selectedE);
		}
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		if (_SceneState == SceneState::EDIT)
		{
			switch (e.GetKey())
			{
			case Key::Q: _GizmoType = ImGuizmo::OPERATION::SCALE; break;
			case Key::E: _GizmoType = ImGuizmo::OPERATION::ROTATE; break;
			case Key::R: _GizmoType = ImGuizmo::OPERATION::TRANSLATE; break;

			case Key::N: if (control) NewScene(); break;
			case Key::O: if (control) OpenScene(); break;
			case Key::S: if (control && shift) SaveSceneAs(); break;
			case Key::D: if (control) OnDuplicateEntity(); break;
			}
		}

		switch (e.GetKey())
		{
		case Key::F1: debugMode = ViewportDebugMode::Default; break;
		case Key::F2: debugMode = ViewportDebugMode::GBuffer_Position; break;
		case Key::F3: debugMode = ViewportDebugMode::GBuffer_Normal; break;
		case Key::F4: debugMode = ViewportDebugMode::GBuffer_Albedo; break;
		case Key::F5: debugMode = ViewportDebugMode::GBuffer_Depth; break;
		case Key::F6: debugMode = ViewportDebugMode::Lighting; break;
		case Key::F7: debugMode = ViewportDebugMode::ShadowMap; break;
		case Key::F8: debugMode = ViewportDebugMode::GI; break;
		}

		if (e.GetKey() == Key::Escape)
		{
			Application::Get()->Close();
			return true;
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		_ActiveScene = MEM::MakeRef<Scene>("New Scene");
		_ActiveScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
		_SceneHierarchyPanel->SetContext(_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = Platform::FileDialog::OpenFile("TAGE Scene (*.tage)\0*.tage\0");
		if (!filepath.empty()) {
			OpenScene(filepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (_SceneState != SceneState::EDIT)
			OnSceneStop();

		if (path.extension().string() != ".tage")
		{
			LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		MEM::Ref<Scene> newScene = MEM::MakeRef<Scene>(path.stem().string());
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			_EditorScene = newScene;
			_EditorScene->OnResize((uint)_ViewportSize.x, (uint)_ViewportSize.y);
			_SceneHierarchyPanel->SetContext(_EditorScene);

			_ActiveScene = newScene;
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = Platform::FileDialog::SaveFile("TAGE Scene (*.tage)\0*.tage\0");
		if (!filepath.empty()) {
			SceneSerializer serializer(_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

	void EditorLayer::SaveLayout()
	{
		std::filesystem::path filepath = Platform::FileDialog::SaveFile("Layout (*.ini)\0*.ini\0");
		if (!filepath.empty()) {
			std::string saveName = (filepath.stem().string() + ".ini");
			ImGui::SaveIniSettingsToDisk(saveName.c_str());
		}
	}

	void EditorLayer::LoadLayout()
	{
		std::filesystem::path filepath = Platform::FileDialog::OpenFile("Layout (*.ini)\0*.ini\0");
		if (!filepath.empty()) {
			std::string layoutName = (filepath.stem().string() + ".ini");
			ImGui::LoadIniSettingsFromDisk(layoutName.c_str());
		}
	}

	void EditorLayer::OnScenePlay()
	{
		_SceneState = SceneState::PLAY;

		_ActiveScene = Scene::Copy(_EditorScene);
		_ActiveScene->OnRuntimeStart();
		_SceneHierarchyPanel->SetContext(_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		_SceneState = SceneState::EDIT;

		_ActiveScene->OnRuntimeStop();
		_ActiveScene = _EditorScene;
		_SceneHierarchyPanel->SetContext(_ActiveScene);
	}

	void EditorLayer::OnSimulateStart()
	{
		_SceneState = SceneState::SIMULATE;

		_ActiveScene = Scene::Copy(_EditorScene);
		_ActiveScene->OnSimulateStart();
		_SceneHierarchyPanel->SetContext(_ActiveScene);
	}

	void EditorLayer::OnSimulateStop()
	{
		_SceneState = SceneState::EDIT;

		_ActiveScene->OnSimulateStop();
		_ActiveScene = _EditorScene;
		_SceneHierarchyPanel->SetContext(_ActiveScene);
	}

	void EditorLayer::UI_BeginDockspace()
	{
		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar(3);

		UI_DrawMainMenuBar();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f });
		UI_DrawToolbar();
		ImGui::PopStyleVar();

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		static bool first_time = true;
		if (first_time)
		{
			first_time = false;
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

			ImGuiID dock_main_id = dockspace_id;
			ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
			ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
			ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.5f, nullptr, &dock_right_id);

			ImGui::DockBuilderDockWindow("Viewport", dock_main_id);
			ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_right_id);
			ImGui::DockBuilderDockWindow("Properties", dock_right_down_id);
			ImGui::DockBuilderDockWindow("Content Browser", dock_down_id);
			ImGui::DockBuilderDockWindow("Output", dock_down_id);
			ImGui::DockBuilderDockWindow("Settings", dock_right_id);

			ImGui::DockBuilderFinish(dockspace_id);
		}
	}

	void EditorLayer::UI_EndDockspace()
	{
		ImGui::End();
	}

	void EditorLayer::UI_DrawMainMenuBar()
	{
		static bool isMaximized = true;
		static bool isDraggingWindow = false;
		static ImVec2 dragStartPos;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 5));
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.12f, 0.12f, 0.13f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N")) NewScene();
				if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) OpenScene();
				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) SaveSceneAs();
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) Application::Get()->Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Duplicate Entity", "Ctrl+D")) OnDuplicateEntity();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Viewport"))
			{
				if (ImGui::MenuItem("Save Layout")) SaveLayout();
				if (ImGui::MenuItem("Load Layout")) LoadLayout();
				ImGui::EndMenu();
			}

			float windowWidth = ImGui::GetWindowWidth();
			const char* title = "Tourqe Advanced Game Engine";
			float titleWidth = ImGui::CalcTextSize(title).x;
			ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
			ImGui::TextUnformatted(title);

			float buttonHeight = ImGui::GetFrameHeight() * 0.8f;
			float buttonWidth = buttonHeight * 1.2f;
			ImVec2 buttonSize = { buttonWidth, buttonHeight };

			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float rightPadding = 10.0f;

			float totalButtonsWidth = (buttonWidth * 3.0f) + (spacing * 2.0f);
			ImGui::SameLine(ImGui::GetWindowWidth() - totalButtonsWidth - rightPadding);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetFrameHeight() - buttonHeight) * 0.5f - 1.0f);

			if (ImGui::Button("-", buttonSize))
			{
				//Application::Get()->GetWindow()->IconifyWindow();
			}
			ImGui::SameLine();

			if (ImGui::Button(isMaximized ? "[]" : "[]", buttonSize))
			{
				Application::Get()->GetWindow()->RestoreWindow(isMaximized);
				isMaximized = !isMaximized;
			}
			ImGui::SameLine();

			if (ImGui::Button("X", buttonSize))
			{
				Application::Get()->Close();
			}

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				isDraggingWindow = false;

			if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				if (!isDraggingWindow)
				{
					isDraggingWindow = true;
					dragStartPos = ImGui::GetMousePos();
				}
			}

			if (isDraggingWindow)
			{
				ImVec2 currentPos = ImGui::GetMousePos();
				ImVec2 delta = ImVec2(currentPos.x - dragStartPos.x, currentPos.y - dragStartPos.y);
				dragStartPos = currentPos;

				int x, y;
				Application::Get()->GetWindow()->GetWindowPos(&x, &y);
				Application::Get()->GetWindow()->SetWindowPos(x + (int)delta.x, y + (int)delta.y);
			}

			ImGui::EndMainMenuBar();
		}

		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);
	}

	void EditorLayer::UI_DrawToolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 2.0f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.13f, 1.0f));

		ImGui::BeginChild("##toolbar", ImVec2(0, 32.0f), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImVec2 buttonSize = { 20.0f, 20.0f };
		float totalButtonsWidth = (buttonSize.x * 2) + ImGui::GetStyle().ItemSpacing.x;

		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - totalButtonsWidth) * 0.5f);
		ImGui::SetCursorPosY((ImGui::GetWindowHeight() - buttonSize.y) * 0.5f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.25f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.45f, 1.0f));

		MEM::Ref<TARE::Texture2D> playIcon = (_SceneState == SceneState::EDIT || _SceneState == SceneState::SIMULATE) ? _PlayIcon : _StopIcon;
		if (ImGui::ImageButton("PlayStop", (ImTextureID)playIcon->GetID(), buttonSize))
		{
			if (_SceneState == SceneState::EDIT || _SceneState == SceneState::SIMULATE)
				OnScenePlay();
			else if (_SceneState == SceneState::PLAY)
				OnSceneStop();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip(_SceneState == SceneState::PLAY ? "Stop (Play Mode)" : "Play Scene");

		ImGui::SameLine();

		MEM::Ref<TARE::Texture2D> simulateIcon = (_SceneState == SceneState::EDIT || _SceneState == SceneState::PLAY) ? _PlayIcon : _StopIcon;
		if (ImGui::ImageButton("SimulateStop", (ImTextureID)simulateIcon->GetID(), buttonSize))
		{
			if (_SceneState == SceneState::EDIT || _SceneState == SceneState::PLAY)
				OnSimulateStart();
			else if (_SceneState == SceneState::SIMULATE)
				OnSimulateStop();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip(_SceneState == SceneState::SIMULATE ? "Stop (Simulate Mode)" : "Simulate Scene");

		ImGui::PopStyleColor(3);
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}

	void EditorLayer::UI_DrawViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		_ViewportFocused = ImGui::IsWindowFocused();
		_ViewportHovered = ImGui::IsWindowHovered();

		if (_ViewportHovered && Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
			Application::Get()->GetWindow()->ToggleCursor(false);
			_ViewportMouseFocused = true;
		}
		else if (Input::IsMouseButtonJustReleased(Mouse::ButtonRight)) {
			Application::Get()->GetWindow()->ToggleCursor(true);
			_ViewportMouseFocused = false;
		}

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		auto renderer = Application::Get()->GetRenderer();
		auto deferred = renderer->GetDeferredRendering();
		uint64_t textureID = deferred.GetLightingBuffer()->GetColorAttachment(0);

		switch (debugMode)
		{
		case ViewportDebugMode::GBuffer_Position: textureID = deferred.GetGBuffer()->GetColorAttachment(0); break;
		case ViewportDebugMode::GBuffer_Normal:   textureID = deferred.GetGBuffer()->GetColorAttachment(1); break;
		case ViewportDebugMode::GBuffer_Albedo:   textureID = deferred.GetGBuffer()->GetColorAttachment(2); break;
		case ViewportDebugMode::GBuffer_Depth:    textureID = deferred.GetGBuffer()->GetDepthAttachment(); break;
		case ViewportDebugMode::Lighting:         textureID = deferred.GetLightingBuffer()->GetColorAttachment(0); break;
		case ViewportDebugMode::ShadowMap:        textureID = renderer->GetSceneData().Lights[0].shadowMap->GetTextureID(); break;
		case ViewportDebugMode::GI:               textureID = deferred.GetGIBuffer()->GetColorAttachment(0); break;
		default: break;
		}
		ImGui::Image((void*)(uintptr_t)textureID, ImVec2(_ViewportSize.x, _ViewportSize.y), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const char* path = (const char*)payload->Data;
				OpenScene(path);
			}
			ImGui::EndDragDropTarget();
		}

		Entity selectedEntity = _SceneHierarchyPanel->GetSelectedEntity();
		if (_SceneState == SceneState::EDIT && selectedEntity && _GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(_ViewportBounds[0].x, _ViewportBounds[0].y, _ViewportBounds[1].x - _ViewportBounds[0].x, _ViewportBounds[1].y - _ViewportBounds[0].y);

			const glm::mat4& cameraProj = _EditorCamera->GetProjectionMatrix();
			glm::mat4 cameraView = _EditorCamera->GetViewMatrix();

			auto& tc = selectedEntity.Transform();
			glm::mat4 transform = _ActiveScene->GetWorldSpaceTransformMatrix(selectedEntity);

			bool snap = Input::IsKeyPressed(Key::LeftShift);
			float snapValue = (_GizmoType == ImGuizmo::OPERATION::ROTATE) ? 45.0f : 0.5f;
			float snapValues[3] = { snapValue, snapValue, snapValue };

			if (ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProj),
				(ImGuizmo::OPERATION)_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr))
			{
				Entity parent = _ActiveScene->TryGetEntityWithUUID(selectedEntity.GetParentUUID());
				if (parent)
				{
					glm::mat4 parentTransform = _ActiveScene->GetWorldSpaceTransformMatrix(parent);
					transform = glm::inverse(parentTransform) * transform;
				}

				glm::vec3 translation, scale;
				glm::quat rotation;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				switch (_GizmoType)
				{
				case ImGuizmo::TRANSLATE:
				{
					tc.Position = translation;
					break;
				}
				case ImGuizmo::ROTATE:
				{
					glm::vec3 originalRotationEuler = tc.GetRotationEuler();

					originalRotationEuler.x = fmodf(originalRotationEuler.x + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
					originalRotationEuler.y = fmodf(originalRotationEuler.y + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
					originalRotationEuler.z = fmodf(originalRotationEuler.z + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();

					glm::vec3 deltaRotationEuler = glm::eulerAngles(rotation) - originalRotationEuler;

					if (fabs(deltaRotationEuler.x) < 0.001) deltaRotationEuler.x = 0.0f;
					if (fabs(deltaRotationEuler.y) < 0.001) deltaRotationEuler.y = 0.0f;
					if (fabs(deltaRotationEuler.z) < 0.001) deltaRotationEuler.z = 0.0f;

					tc.SetRotationEuler(tc.GetRotationEuler() += deltaRotationEuler);
					break;
				}
				case ImGuizmo::SCALE:
				{
					tc.Scale = scale;
					break;
				}
				}
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::UI_DrawSettingsPanel()
	{
		ImGui::Begin("Settings");

		static bool drawColliders = false;
		if (ImGui::Checkbox("Draw Colliders", &drawColliders))
		{
			TARE::RenderAPI::SetRenderMode(drawColliders ? DebugRenderMode::BOUNDING_BOX : DebugRenderMode::NONE);
		}

		const char* drawTypeString[] = { "Default", "GBuffer Position", "GBuffer Normal", "GBuffer Albedo", "GBuffer Depth", "Lighting", "ShadowMap", "GI" };
		int currentDrawType = static_cast<int>(debugMode);
		if (ImGui::Combo("Debug Viewport", &currentDrawType, drawTypeString, IM_ARRAYSIZE(drawTypeString)))
		{
			debugMode = static_cast<ViewportDebugMode>(currentDrawType);
		}

		ImGui::End();
	}
}
