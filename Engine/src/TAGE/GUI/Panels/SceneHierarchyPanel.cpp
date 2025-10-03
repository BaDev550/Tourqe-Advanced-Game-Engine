#include "tagepch.h"
#include "SceneHierarchyPanel.h"
#include "TAGE/World/Components/Components.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

namespace TAGE {
	SceneHierarchyPanel::SceneHierarchyPanel(const MEM::Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const MEM::Ref<Scene>& context)
	{
		_Context = context;
		_SelectionContext = {};
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		_SelectionContext = entity;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (_Context)
		{
			if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					_Context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}

			_Context->GetRegistry().view<IdentityComponent, TransformComponent>().each(
				[&](entt::entity entityID, IdentityComponent& id, TransformComponent& transform)
				{
					Entity entity = _Context->GetEntityByUUID(id.UniqeId);
					DrawEntityNode(entity);
				});


			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				_SelectionContext = {};
		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (_SelectionContext) { DrawComponents(_SelectionContext); }
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		bool entityDeleted = false;
		auto& tag = entity.GetComponent<IdentityComponent>().Name;
		ImGuiTreeNodeFlags flags = ((_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			if (ImGui::MenuItem("Unparent"))
				_Context->UnparentEntity(entity);

			ImGui::EndPopup();
		}

		if (ImGui::IsItemClicked()) {
			_SelectionContext = entity;
		}
	
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY"))
			{
				Entity childEntity = *(Entity*)payload->Data;
				_Context->ParentEntity(childEntity, entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("HIERARCHY_ENTITY", &entity, sizeof(Entity));
			ImGui::Text("%s", tag.c_str());
			ImGui::EndDragDropSource();
		}

		auto& rc = entity.GetComponent<RelationshipComponent>();
		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			for (UUID childUUID : rc.Children)
			{
				Entity childEntity = _Context->GetEntityByUUID(childUUID);
				if (childEntity)
					DrawEntityNode(childEntity);
			}
			ImGui::TreePop();
		}

		if (entityDeleted) {
			_Context->DestroyEntity(entity);
			if (_SelectionContext == entity)
				_SelectionContext = {};
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<IdentityComponent>())
		{
			auto& tag = entity.GetComponent<IdentityComponent>().Name;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("+"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<MeshComponent>("Mesh");
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<RigidBodyComponent>("Rigidbody");
			DisplayAddComponentEntry<ColliderComponent>("Collider");
			DisplayAddComponentEntry<DirectionalLightComponent>("Direct Light");
			DisplayAddComponentEntry<PointLightComponent>("Point Light");
			DisplayAddComponentEntry<SpotLightComponent>("Spot Light");

			ImGui::EndPopup();
		}

		ImGui::Text("Uniqe ID: %s", std::to_string(entity.GetComponent<IdentityComponent>().UniqeId).c_str());

		if (entity.GetParent()) {
			if (ImGui::Button("Unparent")) {
				_Context->UnparentEntity(entity);
			}
		}

		DisplayComponentSettings(entity);
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
}