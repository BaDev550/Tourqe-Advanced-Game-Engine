#include "tagepch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"
#include "TAGE/World/Scene/Scene.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/Input/Input.h"
#include "TAGE/World/Components/Components.h"

#include "mono/jit/jit.h"
#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

namespace TAGE {
#define ADD_INTERNAL_CALL(icall) mono_add_internal_call("TAGE.InternalCalls::"#icall, (void*)icall)

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_HasComponentFuncs;

#pragma region LOG
	static void NativeLog(int logLevel, MonoString* string) {
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);

		switch (logLevel)
		{
		case 0:
			APP_LOG_INFO(str);
			break;
		case 1:
			APP_LOG_WARN(str);
			break;
		case 2:
			APP_LOG_ERROR(str);
			break;
		default:
			APP_LOG_INFO(str);
			break;
		}
	}

	static void NativeLog_Vector3(int logLevel, glm::vec3* vector) {
		switch (logLevel)
		{
		case 0:
			APP_LOG_INFO(glm::to_string(*vector));
			break;
		case 1:
			APP_LOG_WARN(glm::to_string(*vector));
			break;
		case 2:
			APP_LOG_ERROR(glm::to_string(*vector));
			break;
		default:
			APP_LOG_INFO(glm::to_string(*vector));
			break;
		}
	}
#pragma endregion

#pragma region Component
	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation) {
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		*outTranslation = entity.GetComponent<TransformComponent>().Position;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outRotation) {
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		*outRotation = entity.GetComponent<TransformComponent>().GetRotationEuler();
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* Translation) {
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TransformComponent>().Position = *Translation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* Rotation) {
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TransformComponent>().SetRotationEuler(*Rotation);
	}

	static void RigidBodyComponent_ApplyForce(UUID entityID, glm::vec3* force) {
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		rb.AddForce(*force);
	}
	static void RigidBodyComponent_SetVelocity(UUID entityID, glm::vec3* newVelocity) {
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		auto& rb = entity.GetComponent<RigidBodyComponent>();
		rb.SetVelocity(*newVelocity);
	}
#pragma endregion

#pragma region Input
	static bool Input_IsKeyDown(KeyCode keycode) {
		return Input::IsKeyPressed(keycode);
	}
	static void Input_GetMousePos(glm::vec2* pos) {
		*pos = Input::GetMousePosition();
	}
	static void Input_GetMouseDelta(glm::vec2* delta) {
		*delta = Input::GetMouseDelta();
	}
#pragma endregion

#pragma region Entity
	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType) {
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		
		MonoType* managedType = mono_reflection_type_get_type(componentType);
		ASSERT_NOMSG(s_HasComponentFuncs.find(managedType) != s_HasComponentFuncs.end());
		return s_HasComponentFuncs.at(managedType)(entity);
	}
#pragma endregion

	template<typename... Component>
	static void RegisterComponent() {
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypenamme = fmt::format("TAGE.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypenamme.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType) {
					LOG_ERROR("Component type is not founded: {}", managedTypenamme);
					return;
				}
				s_HasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}
	
	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>) {
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		ADD_INTERNAL_CALL(NativeLog);
		ADD_INTERNAL_CALL(NativeLog_Vector3);

		ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		ADD_INTERNAL_CALL(TransformComponent_SetRotation);

		ADD_INTERNAL_CALL(RigidBodyComponent_ApplyForce);
		ADD_INTERNAL_CALL(RigidBodyComponent_SetVelocity);

		ADD_INTERNAL_CALL(Input_IsKeyDown);
		ADD_INTERNAL_CALL(Input_GetMousePos);
		ADD_INTERNAL_CALL(Input_GetMouseDelta);

		ADD_INTERNAL_CALL(Entity_HasComponent);
	}
}