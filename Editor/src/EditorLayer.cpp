#include "EditorLayer.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_internal.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TAGE/World/Scene/SceneSerializer.h"
#include "TAGE/Utilities/Platform.h"

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
		Toolbar();

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

		_SceneHierarchyPanel->OnImGuiRender();
		_ContentBrowserPanel->OnImGuiRender();
		_OutputPanel->OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
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
		
		auto renderer = Application::Get()->GetRenderer();
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
			textureID = renderer->GetSceneData().Lights[0].shadowMap->GetTextureID();
			break;
		case ViewportDebugMode::GI:
			textureID = deferred.GetGIBuffer()->GetColorAttachment(0);
			break;
		}

		ImGui::Image((ImTextureID)(void*)(uintptr_t)textureID, ImVec2{ _ViewportSize.x, _ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(path);
			}
			ImGui::EndDragDropTarget();
		}

		if (Input::IsKeyJustPressed(Key::E)) _GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		else if (Input::IsKeyJustPressed(Key::R)) _GizmoType = ImGuizmo::OPERATION::ROTATE;
		else if (Input::IsKeyJustPressed(Key::Q)) _GizmoType = ImGuizmo::OPERATION::SCALE;

		if (_ViewportHovered && Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
			Application::Get()->GetWindow()->ToggleCursor(false);
			_ViewportMouseFocused = true;
		}
		else {
			Application::Get()->GetWindow()->ToggleCursor(true);
			_ViewportMouseFocused = false;
		}

		Entity selectedEntity = _SceneHierarchyPanel->GetSelectedEntity();
		if (_SceneState == SceneState::EDIT && selectedEntity && _GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);

			auto rect = ImGui::GetWindowSize();
			auto pos = ImGui::GetWindowPos();
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(pos.x, pos.y, rect.x, rect.y);

			//auto cameraEntity = _ActiveScene->GetPrimaryCamera();
			//const auto& camera = cameraEntity.GetComponent<CameraComponent>().Handle;

			//const glm::mat4& cameraProj = camera->GetProjectionMatrix();
			//glm::mat4 cameraView = camera->GetViewMatrix();

			const glm::mat4& cameraProj = _EditorCamera->GetProjectionMatrix();
			glm::mat4 cameraView = _EditorCamera->GetViewMatrix();

			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			bool snap = Input::IsKeyPressed(Key::LeftShift);
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

		ImGui::Begin("Settings");
		static bool drawColliders = false;
		ImGui::Checkbox("Draw Colliders", &drawColliders);
		const char* drawTypeString[] = { 
			"Default", 
			"GBuffer_Position", 
			"GBuffer_Normal",
			"GBuffer_Albedo",
			"GBuffer_Depth",
			"Lighting",
			"ShadowMap",
			"GI"
		};
		int currentdrawType = static_cast<int>(debugMode);
		if (ImGui::Combo("Debug Viewport Image", &currentdrawType, drawTypeString, IM_ARRAYSIZE(drawTypeString))) {
			debugMode = static_cast<ViewportDebugMode>(currentdrawType);
		}

		if (drawColliders) {
			TARE::RenderAPI::SetRenderMode(DebugRenderMode::BOUNDING_BOX);
		}
		else {
			TARE::RenderAPI::SetRenderMode(DebugRenderMode::NONE);
		}

		ImGui::End();

		ImGui::End();
	}

	void EditorLayer::Toolbar()
	{
		static bool isDraggingWindow = false;
		static ImVec2 dragStartPos;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("New");
				if (ImGui::MenuItem("Open Scene...")) {
					OpenScene();
				}
				if (ImGui::MenuItem("Save Scene As...")) {
					SaveSceneAs();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Exit"))
				{
					if (ImGui::MenuItem("Exit")) Application::Get()->Close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::MenuItem("Undo");
				ImGui::MenuItem("Redo");
				ImGui::Separator();
				ImGui::MenuItem("Cut");
				ImGui::MenuItem("Copy");
				if (ImGui::MenuItem("Duplicate Entity")) {

				}
				ImGui::EndMenu();
			}

			const char* text = "Tourqe Advanced Game Engine";
			float windowWidth = ImGui::GetWindowWidth();
			ImVec2 textSize = ImGui::CalcTextSize(text);
			float textPosX = (windowWidth - textSize.x) * 0.5f;

			ImGui::SetCursorPosX(textPosX);
			ImGui::Text("%s", text);

			ImGui::SameLine(ImGui::GetWindowWidth() - 100.0f);

			if (ImGui::Button("-")) {
				//Application::Get()->GetWindow()->IconifyWindow();
			}
			ImGui::SameLine();

			static bool maximized = true;
			if (ImGui::Button("[]")) {
				Application::Get()->GetWindow()->RestoreWindow(maximized);
				maximized = !maximized;
			}
			ImGui::SameLine();

			if (ImGui::Button("X")) {
				Application::Get()->Close();
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
			{
				isDraggingWindow = true;
				dragStartPos = ImGui::GetMousePos();
			}

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
			{
				Application::Get()->GetWindow()->RestoreWindow(maximized);
				maximized = !maximized;
			}

			if (isDraggingWindow)
			{
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
				{
					ImVec2 currentPos = ImGui::GetMousePos();
					ImVec2 delta = ImVec2(currentPos.x - dragStartPos.x, currentPos.y - dragStartPos.y);
					dragStartPos = currentPos;

					int x, y;
					Application::Get()->GetWindow()->GetWindowPos(&x, &y);
					Application::Get()->GetWindow()->SetWindowPos(x + (int)delta.x, y + (int)delta.y);
				}

				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				{
					isDraggingWindow = false;
				}
			}

			ImGui::EndMainMenuBar();
		}

		ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
		float height = ImGui::GetFrameHeight();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		if (ImGui::BeginViewportSideBar("##toolbar", viewport, ImGuiDir_Up, height, window_flags)) {
			if (ImGui::BeginMenuBar()) {
				float iconSize = 20.0f;
				float spacing = ImGui::GetStyle().ItemSpacing.x;
				float totalWidth = iconSize * 3 + spacing * 2;
				float centerPosX = (ImGui::GetWindowWidth() - totalWidth) * 0.5f;

				ImGui::SetCursorPosX(centerPosX);
				ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (iconSize * 0.5f));

				MEM::Ref<TARE::Texture2D> icon = _SceneState == SceneState::PLAY ? _StopIcon : _PlayIcon;
				MEM::Ref<TARE::Texture2D> simulateIcon = _SceneState == SceneState::SIMULATE ? _StopIcon : _PlayIcon;
				if (ImGui::ImageButton("PlayStop", (ImTextureID)icon->GetID(), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1))) {
					if (_SceneState == SceneState::EDIT)
						OnScenePlay();
					else if (_SceneState == SceneState::PLAY)
						OnSceneStop();
				}
				ImGui::SameLine();
				if (ImGui::ImageButton("Simulate", (ImTextureID)simulateIcon->GetID(), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1))) {
					if (_SceneState == SceneState::EDIT)
						OnSimulateStart();
					else if (_SceneState == SceneState::SIMULATE)
						OnSimulateStop();
				}

				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
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

		case Key::S: {
			if (control && shift)
				SaveSceneAs();
			break;
		}
		case Key::N: {
			if (control)
				NewScene();
			break;
		}
		case Key::O: {
			if (control)
				OpenScene();
			break;
		}
		case Key::D: {
			if (control)
				OnDuplicateEntity();
			break;
		}

		default:
			break;
		}

		if (e.GetKey() == Key::Escape)
		{
			Application::Get()->Close();
			return true;
		}
		return true;
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
}
