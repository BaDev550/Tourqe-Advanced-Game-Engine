#pragma once

#include "TAGE/Application/Application.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Scene/Scene.h"

namespace TourqeEditor {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const TAGE::MEM::Ref<TAGE::Scene>& scene);

		void SetContext(const TAGE::MEM::Ref<TAGE::Scene>& scene);

		void OnImGuiRender();

		TAGE::Entity* GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(TAGE::Entity* entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
	
		void DrawEntityNode(TAGE::Entity* entity);
		void DrawComponents(TAGE::Entity* entity);
	private:
		TAGE::MEM::Ref<TAGE::Scene> m_Context;
		TAGE::Entity* m_SelectionContext = nullptr;
	};

}
