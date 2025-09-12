#pragma once

#include "TAGE/Application/Application.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Scene/Scene.h"

namespace TAGE {
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const MEM::Ref<Scene>& scene);

		void SetContext(const MEM::Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return _SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
		void DisplayComponentSettings(Entity entity);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		MEM::Ref<Scene> _Context;
		Entity _SelectionContext;
	};
}