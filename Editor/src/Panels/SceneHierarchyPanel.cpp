#include "SceneHierarchyPanel.h"
#include "TAGE/World/Components/Components.h"
#include "TAGE/Scripting/ScriptEngine.h"
#include "TAGE/Utilities/Platform.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

#ifdef _MSVC_LANG
  #define _CRT_SECURE_NO_WARNINGS
#endif

namespace TAGE::Editor {

	SceneHierarchyPanel::SceneHierarchyPanel(const MEM::Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const MEM::Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->GetRegistry().view<IdentityComponent, TransformComponent>().each(
				[&](entt::entity entityID, IdentityComponent& id, TransformComponent& transform)
				{
					Entity entity = m_Context->GetEntityByUUID(id.UniqeId);
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

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<IdentityComponent>().Name;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY"))
			{
				Entity childEntity = *(Entity*)payload->Data;
				m_Context->ParentEntity(childEntity, entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("HIERARCHY_ENTITY", &entity, sizeof(Entity));
			ImGui::Text("%s", tag.c_str());
			ImGui::EndDragDropSource();
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			if (ImGui::MenuItem("Unparent"))
			{
				m_Context->UnparentEntity(entity);
			}

			ImGui::EndPopup();
		}
		auto& rc = entity.GetComponent<RelationshipComponent>();

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			for (UUID childUUID : rc.Children)
			{
				Entity childEntity = m_Context->GetEntityByUUID(childUUID);
				if (childEntity)
				{
					DrawEntityNode(childEntity);
				}
			}
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
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
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
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
				entity.RemoveComponent<T>();
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

			if (ImGui::Button("Delete")) {
				m_Context->DestroyEntity(entity);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<LightComponent>("Light");
			DisplayAddComponentEntry<MeshComponent>("Mesh");
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<SkyboxComponent>("Skybox");
			DisplayAddComponentEntry<RigidBodyComponent>("Rigidbody");
			DisplayAddComponentEntry<ColliderComponent>("Collider");

			ImGui::EndPopup();
		}

		ImGui::Text("Uniqe ID: %s", std::to_string(entity.GetComponent<IdentityComponent>().UniqeId).c_str());

		if (entity.GetParent()) {
			if (ImGui::Button("Unparent")) {
				m_Context->UnparentEntity(entity);
			}
		}

		ImGui::PopItemWidth();
		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Position);
				glm::vec3 eulerDegrees = component.GetRotationEuler();
				if (DrawVec3Control("Rotation", eulerDegrees))
					component.SetRotationEuler(eulerDegrees);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<ScriptComponent>("Script", entity, [entity, this](auto& component) mutable
			{
				bool scriptClassExist = ScriptEngine::EntityClassExists(component.Name);
				
				static char buffer[64];
				strcpy(buffer, component.Name.c_str());

				if (!scriptClassExist)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9, 0.1, 0.3, 1.0f));

				if (ImGui::InputText("Class", buffer, sizeof(buffer)))
					component.Name = buffer;

				if (m_Context->IsRunning()) {
					MEM::Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
					if (scriptInstance) {
						const auto& fields = scriptInstance->GetScriptClass()->GetFields();
						for (const auto& [name, field] : fields) {
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptInstance->GetFieldValue<float>(name);
								if (ImGui::DragFloat(name.c_str(), &data)) {
									scriptInstance->SetFieldValue<float>(name, data);
								}
							}
						}
					}
				}
				else {
					if (scriptClassExist) {
						MEM::Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.Name);
						const auto& fields = entityClass->GetFields();
						auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

						for (const auto& [name, field] : fields) {
							if (entityFields.find(name) != entityFields.end()) {
								ScriptFieldInstance& scriptField = entityFields.at(name);

								if (field.Type == ScriptFieldType::Float) {
									float data = scriptField.GetValue<float>();
									if (ImGui::DragFloat(name.c_str(), &data))
										scriptField.SetValue<float>(data);
								}
							}
							else {
								if (field.Type == ScriptFieldType::Float) {
									float data = 0.0f;
									if (ImGui::DragFloat(name.c_str(), &data)) {
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue(data);
									}
								}
							}
						}
					}
				}

				if (!scriptClassExist)
					ImGui::PopStyleColor();
			});

		DrawComponent<MeshComponent>("Mesh", entity, [](auto& component)
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
					std::string selectedMesh = Platform::FileDialog::OpenFile("Mesh Files (*.obj;*.gltf)\0*.obj;*.gltf\0");
					if (!selectedMesh.empty())
					{
						if (!component.Handle)
							component.Handle = MEM::MakeRef<TARE::Model>();

						component.LoadMesh(selectedMesh);
					}
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const char* path = static_cast<const char*>(payload->Data);
						std::filesystem::path fPath(path);

						if (fPath.extension() == L".obj" || fPath.extension() == L".fbx") {
							if (!component.Handle)
								component.Handle = MEM::MakeRef<TARE::Model>();

							component.LoadMesh(fPath.string());
							LOG_INFO("Path: {}", fPath.string());
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::Checkbox("Visible", &component.IsVisible);
				ImGui::Checkbox("Cast Shadows", &component.CastShadows);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
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

		DrawComponent<SkyboxComponent>("Skybox", entity, [](auto& component)
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
					std::string selectedSkybox = Platform::FileDialog::OpenFile("Cubemap Files (*.hdr;*.png;*.exr)\0*.hdr;*.exr;*.png\0");
					if (!selectedSkybox.empty())
					{
						component.Handle = MEM::MakeRef<TARE::Skybox>(selectedSkybox.c_str());
					}
				}
			});

		DrawComponent<LightComponent>("Light", entity, [](auto& component)
			{
				DrawVec3Control("Color", component.Handle.color);
				ImGui::Checkbox("Cast Shadow", &component.Handle.castShadow);
				ImGui::DragFloat("Intensity", &component.Handle.intensity, 0.01f, 0.0f, 100.0f);
				ImGui::DragFloat("Range", &component.Handle.range, 0.1f, 0.0f, 400.0f);
				ImGui::DragFloat("Inner Cone", &component.Handle.innerCone, 0.1f, 0.0f, glm::radians(90.0f));
				ImGui::DragFloat("Outer Cone", &component.Handle.outerCone, 0.1f, 0.0f, glm::radians(90.0f));
				ImGui::Combo("Type", (int*)&component.Handle.type, "Point\0Directional\0Spot\0");
			});

		DrawComponent<RigidBodyComponent>("Rigid Body", entity, [&](auto& component)
			{
				if (entity.HasComponent<ColliderComponent>()) {
					ImGui::Text("Body Pointer: 0x%p", component.Body);
					ImGui::Text("Motion State Pointer: 0x%p", component.MotionState);

					const char* bodyTypeStrings[] = { "Static", "Kinematic", "Dynamic" };
					int currentType = static_cast<int>(component.BodyType);
					if (ImGui::Combo("Body Type", &currentType, bodyTypeStrings, IM_ARRAYSIZE(bodyTypeStrings))) {
						component.BodyType = static_cast<PhysicsBodyType>(currentType);
					}

					if (ImGui::CollapsingHeader("Constant Movement")) {
						static bool lockX, lockY, lockZ = false;
						component.MovementLocked(lockX, lockY, lockZ);

						static bool valueChanged = false;
						ImGui::Text("Lock Movement");
						ImGui::SameLine();
						if (ImGui::Checkbox("X##movementX", &lockX))
							valueChanged = true;
						ImGui::SameLine();
						if (ImGui::Checkbox("Y##movementY", &lockY))
							valueChanged = true;
						ImGui::SameLine();
						if (ImGui::Checkbox("Z##movementZ", &lockZ))
							valueChanged = true;

						if (valueChanged) {
							component.LockMovement(lockX, lockY, lockZ);
							valueChanged = false;
						}
					}

					if (ImGui::CollapsingHeader("Constant Rotation")) {
						static bool lockX, lockY, lockZ = false;
						component.RotationLocked(lockX, lockY, lockZ);

						static bool valueChanged = false;
						ImGui::Text("Lock Rotation");
						ImGui::SameLine();
						if (ImGui::Checkbox("X##rotationX", &lockX))
							valueChanged = true;
						ImGui::SameLine();
						if (ImGui::Checkbox("Y##rotationY", &lockY))
							valueChanged = true;
						ImGui::SameLine();
						if (ImGui::Checkbox("Z##rotationZ", &lockZ))
							valueChanged = true;

						if (valueChanged) {
							component.LockRotation(lockX, lockY, lockZ);
							valueChanged = false;
						}
					}
				}
				else {
					ImGui::Text("You need to add collider component");
				}
			});

		DrawComponent<ColliderComponent>("Collider", entity, [](auto& component)
			{
				DrawVec3Control("Offset", component.Offset);
				DrawVec3Control("Size", component.Size);

				const char* shapeStrings[] = { "Box", "Sphere", "Capsule", "Mesh" };
				int currentShape = static_cast<int>(component.Shape);
				if (ImGui::Combo("Shape", &currentShape, shapeStrings, IM_ARRAYSIZE(shapeStrings))) {
					component.Shape = static_cast<ColliderShapeType>(currentShape);
					component.Dirty = true;
				}

				ImGui::Checkbox("Is Trigger", reinterpret_cast<bool*>(&component.ResponseType));
				if (component.IsTrigger())
					component.ResponseType = CollisionResponseType::OVERLAP;
				else
					component.ResponseType = CollisionResponseType::BLOCK;

			});
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

}
