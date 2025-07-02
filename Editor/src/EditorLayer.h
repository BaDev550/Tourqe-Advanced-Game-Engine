#pragma once

#include "TAGE/TAGE.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/OutputPanel.h"

namespace TAGE::Editor {
	enum class ViewportDebugMode
	{
		Default = 0,
		GBuffer_Position,
		GBuffer_Normal,
		GBuffer_Albedo,
		GBuffer_Depth,
		Lighting,
		ShadowMap,
		GI
	};

	enum class SceneState
	{
		EDIT = 0,
		PLAY = 1, 
		SIMULATE = 2
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(float dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
		void OnDuplicateEntity();
	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path&);
		void SaveSceneAs();
		void SaveScene();

		void SaveLayout();
		void LoadLayout();

		void OnScenePlay();
		void OnSceneStop();
		void OnSimulateStart();
		void OnSimulateStop();

		void UI_BeginDockspace();
		void UI_EndDockspace();
		void UI_DrawMainMenuBar();
		void UI_DrawToolbar();
		void UI_DrawViewport();
		void UI_DrawSettingsPanel();
	private:
		Entity* _SelectedObject;
		MEM::Ref<TARE::EditorCamera> _EditorCamera;
		MEM::Ref<Scene> _ActiveScene;
		MEM::Ref<Scene> _EditorScene;
		std::filesystem::path _EditorSavePath;

		bool _ViewportFocused = false, _ViewportHovered = false, _ViewportMouseFocused = true;
		glm::vec2 _ViewportSize = { 0.0f, 0.0f };
		glm::vec2 _ViewportBounds[2];
		glm::vec2 _LastViewportSize;

		int _HoveredEntityID = 0;
		int _GizmoType = -1;
		bool _HoveringGizmo = false;
		SceneState _SceneState = SceneState::EDIT;
		ViewportDebugMode debugMode = ViewportDebugMode::Default;

		MEM::Scope<SceneHierarchyPanel> _SceneHierarchyPanel;
		MEM::Scope<ContentBrowserPanel> _ContentBrowserPanel;
		MEM::Scope<OutputPanel> _OutputPanel;

		MEM::Ref<TARE::Texture2D> _PlayIcon;
		MEM::Ref<TARE::Texture2D> _StopIcon;
		MEM::Ref<TARE::Texture2D> _SimulateIcon;

		MEM::Ref<TARE::Texture2D> _IconTranslate;
		MEM::Ref<TARE::Texture2D> _IconRotate;
		MEM::Ref<TARE::Texture2D> _IconScale;
	};

}
