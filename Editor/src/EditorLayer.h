#pragma once

#include "TAGE/Layers/Layer.h"
#include "TAGE/Application/Application.h"
#include "TAGE/Utilities/Timestep.h"
#include "TAGE/Input/KeyCodes.h"
#include "TAGE/Input/MouseCodes.h"
#include "TAGE/Events/Event.h"
#include "TAGE/Events/InputEvents.h"
#include "TAGE/World/Objects/Object.h"
#include "Panels/SceneHierarchyPanel.h"

#include "TARE/Camera/FreeCamera.h"

namespace TourqeEditor {

	class EditorLayer : public TAGE::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(float dt) override;
		virtual void OnImGuiRender() override;
	private:
		TARE::FreeCamera _FreeCamera;
		TAGE::Object* _SelectedObject;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		int m_GizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		TAGE::MEM::Scope<SceneHierarchyPanel> m_SceneHierarchyPanel;
		//TAGE::MEM::Scope<ContentBrowserPanel> m_ContentBrowserPanel;
	};

}
