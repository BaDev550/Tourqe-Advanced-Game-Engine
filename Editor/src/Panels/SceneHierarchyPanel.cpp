#include "SceneHierarchyPanel.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/World/Components/RenderComponents.h"
#include "TAGE/Utilities/Platform.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

#ifdef _MSVC_LANG
  #define _CRT_SECURE_NO_WARNINGS
#endif

namespace TourqeEditor {

	SceneHierarchyPanel::SceneHierarchyPanel(const TAGE::MEM::Ref<TAGE::Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const TAGE::MEM::Ref<TAGE::Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->GetRegistry().view<TAGE::IdentityComponent, TAGE::TransformComponent>().each(
				[&](entt::entity entityID, TAGE::IdentityComponent& id, TAGE::TransformComponent& transform)
				{
					TAGE::Entity* entity = &m_Context->GetEntityByID(entityID);
					DrawEntityNode(entity);
				});


			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			if (ImGui::BeginPopupContextWindow(0, 1))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_Context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}

		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(TAGE::Entity* entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(TAGE::Entity* entity)
	{
		auto& tag = entity->GetComponent<TAGE::IdentityComponent>().Name;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_SelectionContext->Destroy();
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		bool valueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) {
			values.x = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
			valueChanged = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) {
			values.y = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
			valueChanged = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) {
			values.z = resetValue;
			valueChanged = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f")){
			valueChanged = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return valueChanged;
	}
	
	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, TAGE::Entity* entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity->HasComponent<T>())
		{
			auto& component = entity->GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity->RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(TAGE::Entity* entity)
	{
		if (entity->HasComponent<TAGE::IdentityComponent>())
		{
			auto& tag = entity->GetComponent<TAGE::IdentityComponent>().Name;

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

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<TAGE::LightComponent>("Light");
			DisplayAddComponentEntry<TAGE::MeshComponent>("Mesh");
			DisplayAddComponentEntry<TAGE::CameraComponent>("Camera");
			DisplayAddComponentEntry<TAGE::SkyboxComponent>("Skybox");
			//DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			//DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			//DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			//DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
			//DisplayAddComponentEntry<TextComponent>("Text Component");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();
		DrawComponent<TAGE::TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Position);
				glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(component.Rotation));
				if (DrawVec3Control("Rotation", eulerDegrees))
					component.Rotation = glm::quat(glm::radians(eulerDegrees));
				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<TAGE::MeshComponent>("Mesh", entity, [](auto& component)
			{
				if (component.Handle)
				{
					ImGui::Text("Mesh Path: %s", component.Handle->GetFilePath().c_str());
				}
				else
				{
					ImGui::Text("Mesh not loaded.");
				}

				if (ImGui::Button("Select Mesh"))
				{
					std::string selectedMesh = TAGE::Platform::FileDialog::OpenFile("Mesh Files (*.obj;*.gltf)\0*.obj;*.gltf\0");
					if (!selectedMesh.empty())
					{
						if (!component.Handle)
							component.Handle = TAGE::MEM::MakeRef<TARE::Model>();

						component.Handle->LoadFromFile(selectedMesh);
					}
				}

				ImGui::Checkbox("Visible", &component.IsVisible);
				ImGui::Checkbox("Cast Shadows", &component.CastShadows);
			});

		DrawComponent<TAGE::CameraComponent>("Camera", entity, [](auto& component)
			{
				ImGui::Checkbox("Active", &component.IsActive);

				float fov = component.Handle->GetFOV();
				if (ImGui::DragFloat("FOV", &fov, 0.1f, 1.0f, 179.0f))
				{
					component.Handle->SetFOV(fov);
				}

				float nearClip = component.Handle->GetNearClip();
				if (ImGui::DragFloat("Near Clip", &nearClip, 0.01f, 0.01f, 100.0f))
				{
					component.Handle->SetNearClip(nearClip);
				}

				float farClip = component.Handle->GetFarClip();
				if (ImGui::DragFloat("Far Clip", &farClip, 0.1f, 0.1f, 10000.0f))
				{
					component.Handle->SetFarClip(farClip);
				}
			});

		DrawComponent<TAGE::SkyboxComponent>("Skybox", entity, [](auto& component)
			{
				if (component.Handle)
				{
					ImGui::Text("Skybox Path: %s", component.Handle->GetTexture()->GetPath().c_str());
				}
				else
				{
					ImGui::Text("Skybox not loaded.");
				}

				if (ImGui::Button("Select Skybox"))
				{
					std::string selectedSkybox = TAGE::Platform::FileDialog::OpenFile("Cubemap Files (*.hdr;*.png;*.exr)\0*.hdr;*.exr;*.png\0");
					if (!selectedSkybox.empty())
					{
						component.Handle = TAGE::MEM::MakeRef<TARE::Skybox>(selectedSkybox.c_str());
					}
				}
			});

		DrawComponent<TAGE::LightComponent>("Light", entity, [](auto& component)
			{
				DrawVec3Control("Color", component.Handle.color);
				ImGui::DragFloat("Intensity", &component.Handle.intensity, 0.01f, 0.0f, 100.0f);
				ImGui::DragFloat("Range", &component.Handle.range, 0.1f, 0.0f, 100.0f);
				ImGui::DragFloat("Inner Cone", &component.Handle.innerCone, 0.1f, 0.0f, glm::radians(90.0f));
				ImGui::DragFloat("Outer Cone", &component.Handle.outerCone, 0.1f, 0.0f, glm::radians(90.0f));
				ImGui::Combo("Type", (int*)&component.Handle.type, "Point\0Directional\0Spot\0");
			});
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext->HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext->AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

}
