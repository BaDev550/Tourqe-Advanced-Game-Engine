#include "tagepch.h"
#include "SceneHierarchyPanel.h"
#include "TAGE/World/Components/Components.h"
#include "TAGE/Scripting/ScriptEngine.h"
#include "TAGE/Utilities/Platform.h"
#include "TAGE/GUI/GUIUtils.h"
#include "TAGE/GUI/Modals/MaterialEditor.h"
#include "TAGE/AssetManager/AssetManager.h"

#include <imgui.h>

namespace TAGE {
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

	void SceneHierarchyPanel::DisplayComponentSettings(Entity entity)
	{
		ImGui::PopItemWidth();
		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
			{
				GUI::DrawVector3Control("Translation", component.Position);
				glm::vec3 eulerDegrees = component.GetRotationEuler();
				if (GUI::DrawVector3Control("Rotation", eulerDegrees))
					component.SetRotationEuler(eulerDegrees);
				GUI::DrawVector3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<ScriptComponent>("Script", entity, [entity, this](ScriptComponent& component) mutable
			{
				bool scriptClassExist = ScriptEngine::EntityClassExists(component.Name);

				static char buffer[64];
				strcpy(buffer, component.Name.c_str());

				if (!scriptClassExist)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9, 0.1, 0.3, 1.0f));

				if (ImGui::InputText("Class", buffer, sizeof(buffer)))
					component.Name = buffer;

				if (_Context->IsRunning()) {
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
								//if (field.Type == ScriptFieldType::Entity) {
								//	MonoObject* entityObject = scriptField.GetValue<MonoObject*>();
								//	uint64 fieldEntityId = 0;

								//	int selectedEntity = 0;
								//	std::vector<std::string> entityNames;
								//	std::vector<UUID> entities;

								//	int i = 0;
								//	_Context->GetRegistry().view<IdentityComponent, TransformComponent>().each(
								//		[&](entt::entity entityID, IdentityComponent& id, TransformComponent& transform)
								//		{
								//			entityNames.push_back(id.Name);
								//			entities.push_back((UUID)entityID);
								//			if (fieldEntityId == (UUID)entityID)
								//				selectedEntity = i;
								//			i++;
								//		});

								//	if (GUI::ComboBox("Select Entity", selectedEntity, entityNames)) {
								//		
								//	}
								//}
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

		DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent& component)
			{
				{
					AssetHandle currentIndex = 0;
					if (GUI::ComboBox("Mesh", currentIndex, AssetType::StaticMesh, component.Handle ? component.Handle->_handle : 0)) {
						if (!component.Handle)
							component.Handle = MEM::MakeRef<TARE::Model>();
						component.LoadMesh(currentIndex);
					}
				}

				if (!component.Handle)
				{
					ImGui::Text("Mesh not loaded.");
				}
				else {
					auto meshes = component.Handle->GetMeshes();
					int meshIndex = 0;
					for (auto& mesh : meshes)
					{
						auto& material = mesh->GetMaterial();
						ImGui::PushID(meshIndex);

						if (ImGui::Button("Edit##Material"))
						{
							MaterialEditor::OpenModal(material->_handle);
						}

						ImGui::SameLine();
						{
#if 0
							AssetHandle currentIndex = 0;
							if (GUI::ComboBox("Material", currentIndex, AssetType::Material, material->_handle)) {
								auto& material = AssetManager::GetAsset<TARE::Material>(currentIndex);
								mesh->SetMaterial(material);
								Project::GetActive()->GetEditorAssetManager()->SaveAsset(component.Handle->_handle);
							}
#endif
						}
						ImGui::PopID();
						meshIndex++;
					}

					ImGui::Checkbox("Visible", &component.IsVisible);
					ImGui::Checkbox("Cast Shadows", &component.CastShadows);
				}
			});

		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
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

		DrawComponent<RigidBodyComponent>("Rigid Body", entity, [&](RigidBodyComponent& component)
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

		DrawComponent<ColliderComponent>("Collider", entity, [](ColliderComponent& component)
			{
				GUI::DrawVector3Control("Offset", component.Offset);
				GUI::DrawVector3Control("Size", component.Size);

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
}