#pragma once

#include "TAGE/Application/Application.h"
#include "TAGE/World/Objects/Object.h"
#include "TAGE/World/Scene/Scene.h"

namespace TourqeEditor {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const TAGE::MEM::Ref<TAGE::Scene>& scene);

		void SetContext(const TAGE::MEM::Ref<TAGE::Scene>& scene);

		void OnImGuiRender();

		TAGE::Object* GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(TAGE::Object* entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
	
		void DrawEntityNode(TAGE::Object* entity);
		void DrawComponents(TAGE::Object* entity);
	private:
		TAGE::MEM::Ref<TAGE::Scene> m_Context;
		TAGE::Object* m_SelectionContext = nullptr;
	};

}
