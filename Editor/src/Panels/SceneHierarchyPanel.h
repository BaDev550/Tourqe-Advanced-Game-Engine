#pragma once

#include "TAGE/Application/Application.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Scene/Scene.h"

namespace TAGE::Editor {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const MEM::Ref<Scene>& scene);

		void SetContext(const MEM::Ref<Scene>& scene);

		void OnImGuiRender();

		Entity* GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity* entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
	
		void DrawEntityNode(Entity* entity);
		void DrawComponents(Entity* entity);
	private:
		MEM::Ref<Scene> m_Context;
		Entity* m_SelectionContext = nullptr;
	};

}
