#include "tagepch.h"
#include "SceneSerializer.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/World/Components/RenderComponents.h"
#include "TAGE/World/Components/ScriptingComponents.h"
#include "TAGE/World/Components/PhysicsComponents.h"
#include "TAGE/Scripting/ScriptEngine.h"
#include "TAGE/Utilities/UUID.h"
#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::quat> {
		static Node encode(const glm::quat& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}
		static bool decode(const Node& node, glm::quat& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<TAGE::UUID> {
		static Node encode(const TAGE::UUID& uuid) {
			Node node;
			node.push_back((uint64)uuid);
			return node;
		}
		static bool decode(const Node& node, TAGE::UUID& uuid) {
			uuid = node[0].as<uint64>();
			return true;
		}
	};
}
namespace TAGE {
#define WRITE_FIELD_DATA(FieldType, Type) case ScriptFieldType::FieldType: \
	out << scriptField.GetValue<Type>(); \
	break;

#define READ_FIELD_DATA(FieldType, Type) \
	case ScriptFieldType::FieldType:\
	{ \
		Type data = scriptField["Data"].as<Type>(); \
		fieldInstance.SetValue(data); \
		break; \
	}

	SceneSerializer::SceneSerializer(const MEM::Ref<Scene>& scene) : _Scene(scene) {}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec) {
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec) {
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec) {
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& quat) {
		out << YAML::Flow;
		out << YAML::BeginSeq << quat.x << quat.y << quat.z << quat.w << YAML::EndSeq;
		return out;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<IdentityComponent>()) {
			out << YAML::Key << "IdentityComponent";
			out << YAML::BeginMap;

			auto& identity = entity.GetComponent<IdentityComponent>();
			out << YAML::Key << "Name" << YAML::Value << identity.Name;

			if (identity.Tags.size() > 0) {
				out << YAML::Key << "Tags" << YAML::Value << YAML::BeginSeq;
				for (const auto& tag : identity.Tags) {
					out << tag;
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap;
		}

		if (entity.HasComponent<RelationshipComponent>())
		{
			auto& relationshipComponent = entity.GetComponent<RelationshipComponent>();
			out << YAML::Key << "Parent" << YAML::Value << relationshipComponent.ParentHandle;

			out << YAML::Key << "Children";
			out << YAML::Value << YAML::BeginSeq;

			for (auto child : relationshipComponent.Children)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << child;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		if (entity.HasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transform.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transform.GetRotationEuler();
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshComponent>()) {
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;
			auto& mesh = entity.GetComponent<MeshComponent>();
			out << YAML::Key << "MeshHandle" << YAML::Value << (uint64)mesh.Handle->_handle;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			auto& camera = entity.GetComponent<CameraComponent>();

			out << YAML::Key << "Camera" << YAML::Value;
				out << YAML::BeginMap;
				out << YAML::Key << "AspectRatio" << YAML::Value << camera.Handle->GetAspectRatio();
				out << YAML::Key << "FOV" << YAML::Value << camera.Handle->GetFOV();
				out << YAML::Key << "NearClip" << YAML::Value << camera.Handle->GetNearClip();
				out << YAML::Key << "FarClip" << YAML::Value << camera.Handle->GetFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "IsActive" << YAML::Value << camera.IsActive;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<LightComponent>()) {
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;
			auto& light = entity.GetComponent<LightComponent>();
			out << YAML::Key << "Type" << YAML::Value << (int)light.Handle.Type;
			out << YAML::Key << "Color" << YAML::Value << light.Handle.Color;
			out << YAML::Key << "Range" << YAML::Value << light.Handle.Range;
			out << YAML::Key << "Intensity" << YAML::Value << light.Handle.Intensity;
			out << YAML::Key << "CastShadow" << YAML::Value << light.Handle.CastShadow;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SkyboxComponent>()) {
			out << YAML::Key << "SkyboxComponent";
			out << YAML::BeginMap;
			auto& skybox = entity.GetComponent<SkyboxComponent>();
			out << YAML::Key << "TexturePath" << YAML::Value << skybox.Handle->GetTexture()->GetPath();
			out << YAML::EndMap;
		}

		if (entity.HasComponent<RigidBodyComponent>()) {
			out << YAML::Key << "RigidBodyComponent";
			out << YAML::BeginMap;
			auto& rb = entity.GetComponent<RigidBodyComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << (int)rb.BodyType;
			out << YAML::Key << "LinearFactor" << YAML::Value << rb.LinearFactor;
			out << YAML::Key << "AngularFactor" << YAML::Value << rb.AngularFactor;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<ColliderComponent>()) {
			out << YAML::Key << "ColliderComponent";
			out << YAML::BeginMap;
			auto& cc = entity.GetComponent<ColliderComponent>();
			
			out << YAML::Key << "Shape" << YAML::Value << (int)cc.Shape;
			out << YAML::Key << "Size" << YAML::Value << cc.Size;
			out << YAML::Key << "ResponseType" << YAML::Value << (int)cc.ResponseType;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<ScriptComponent>()) {
			auto& sc = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Class" << YAML::Value << sc.Name;

			MEM::Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.Name);
			const auto& fields = entityClass->GetFields();

			if (!fields.empty()) {
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& scriptFields = ScriptEngine::GetScriptFieldMap(entity);
				out << YAML::BeginSeq;
				for (const auto& [name, field] : fields) {
					if (scriptFields.find(name) == scriptFields.end())
						continue;

					out << YAML::BeginMap;
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);
					out << YAML::Key << "Data" << YAML::Value;
					ScriptFieldInstance& scriptField = scriptFields.at(name);

					switch (field.Type)
					{
						WRITE_FIELD_DATA(Float,   float);
						WRITE_FIELD_DATA(Int,     int);
						WRITE_FIELD_DATA(Double,  double);
						WRITE_FIELD_DATA(Bool,    bool);
						WRITE_FIELD_DATA(Char,    char);
						WRITE_FIELD_DATA(Byte,    uint8);
						WRITE_FIELD_DATA(Short,   int16);
						WRITE_FIELD_DATA(Long,    int64);
						WRITE_FIELD_DATA(UShort,  uint16);
						WRITE_FIELD_DATA(UInt,    uint);
						WRITE_FIELD_DATA(ULong,   uint64);
						WRITE_FIELD_DATA(Vector2, glm::vec2);
						WRITE_FIELD_DATA(Vector3, glm::vec3);
						WRITE_FIELD_DATA(Vector4, glm::vec4);
						WRITE_FIELD_DATA(Entity,  UUID);
					}
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
			}
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << _Scene->GetName();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		for (auto entityID : _Scene->GetRegistry().view<TransformComponent, IdentityComponent>())
		{
			Entity entity = { entityID, _Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		};
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream file(filepath);
		file << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		LOG_TRACE("Deserializing scene: '{}'", sceneName);

		auto entities = data["Entities"];
		if (entities) {
			for (auto entity : entities) {
				uint64 uuid = entity["Entity"].as<uint64>();

				std::string name;
				auto IdentityComponent = entity["IdentityComponent"];
				if (IdentityComponent) {
					name = IdentityComponent["Name"].as<std::string>();
				}

				LOG_TRACE("Deserializing entity with ID: {}, Name: {}", uuid, name);

				Entity& deserializedEntity = _Scene->CreateEntityWithUUID(name, uuid);
				
				auto& relationshipComponent = deserializedEntity.GetComponent<RelationshipComponent>();
				uint64 parentHandle = entity["Parent"] ? entity["Parent"].as<uint64>() : 0;
				relationshipComponent.ParentHandle = parentHandle;

				auto children = entity["Children"];
				if (children)
				{
					for (auto child : children)
					{
						uint64 childHandle = child["Handle"].as<uint64>();
						relationshipComponent.Children.push_back(childHandle);
					}
				}

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent) {
					auto& transform = deserializedEntity.GetComponent<TransformComponent>();
					transform.Position = transformComponent["Position"].as<glm::vec3>();
					transform.SetRotationEuler(transformComponent["Rotation"].as<glm::vec3>());
					transform.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent) {
					uint64 meshHandle = meshComponent["MeshHandle"].as<uint64>();
					auto& mesh = deserializedEntity.AddOrReplaceComponent<MeshComponent>(meshHandle);
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					auto& cameraData = cameraComponent["Camera"];
					auto& camera = deserializedEntity.AddOrReplaceComponent<CameraComponent>();
					camera.Handle->SetAspectRatio(cameraData["AspectRatio"].as<float>());
					camera.Handle->SetFOV(cameraData["FOV"].as<float>());
					camera.Handle->SetNearClip(cameraData["NearClip"].as<float>());
					camera.Handle->SetFarClip(cameraData["FarClip"].as<float>());
					camera.IsActive = cameraComponent["IsActive"].as<bool>();
				}

				auto lightComponent = entity["LightComponent"];
				if (lightComponent) {
					LightType type = (LightType)lightComponent["Type"].as<int>();
					/*glm::vec4 color = lightComponent["Color"].as<glm::vec4>();*/
					glm::vec4 color = glm::vec4(1.0f);
					float range = lightComponent["Range"].as<float>();
					float intensity = lightComponent["Intensity"].as<float>();
					bool castShadow = lightComponent["CastShadow"].as<bool>();
					auto& light = deserializedEntity.AddOrReplaceComponent<LightComponent>(Light(type, castShadow, intensity, range, {}, {}, color));
				}

				auto skyboxComponent = entity["SkyboxComponent"];
				if (skyboxComponent) {
					auto& texturePath = skyboxComponent["TexturePath"].as<std::string>();
					auto& skybox = deserializedEntity.AddOrReplaceComponent<SkyboxComponent>(texturePath);
				}

				auto colliderComponent = entity["ColliderComponent"];
				if (colliderComponent) {
					auto shape = (ColliderShapeType)colliderComponent["Shape"].as<int>();
					auto size = colliderComponent["Size"].as<glm::vec3>();
					auto response = (CollisionResponseType)colliderComponent["ResponseType"].as<int>();
					auto& collider = deserializedEntity.AddOrReplaceComponent<ColliderComponent>();
					collider.Shape = shape;
					collider.ResponseType = response;
					collider.Size = size;
				}

				auto rigidBodyComponent = entity["RigidBodyComponent"];
				if (rigidBodyComponent) {
					auto bodyType = (PhysicsBodyType)rigidBodyComponent["BodyType"].as<int>();
					auto linearFactor = rigidBodyComponent["LinearFactor"].as<glm::vec3>();
					auto angularFactor = rigidBodyComponent["AngularFactor"].as<glm::vec3>();

					auto& rigidBody = deserializedEntity.AddOrReplaceComponent<RigidBodyComponent>();
					rigidBody.BodyType = bodyType;
					rigidBody.LinearFactor = linearFactor;
					rigidBody.AngularFactor = angularFactor;
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent) {
					auto className = scriptComponent["Class"].as<std::string>();
					auto& script = deserializedEntity.AddOrReplaceComponent<ScriptComponent>(className);

					auto scriptFields = scriptComponent["ScriptFields"];
					if (scriptFields) {
						MEM::Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(script.Name);
						const auto& fields = entityClass->GetFields();
						auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

						for (auto scriptField : scriptFields) {
							std::string name = scriptField["Name"].as<std::string>();
							std::string typeString = scriptField["Type"].as<std::string>();
							std::string data = scriptField["Data"].as<std::string>();
							ScriptFieldType type = Utils::StringToScriptFieldType(typeString);
							ScriptFieldInstance& fieldInstance = entityFields[name];
							if (fields.find(name) == fields.end())
								continue;

							fieldInstance.Field = fields.at(name);

							switch (type)
							{
								READ_FIELD_DATA(Float,   float);
								READ_FIELD_DATA(Int,     int);
								READ_FIELD_DATA(Double,  double);
								READ_FIELD_DATA(Bool,    bool);
								READ_FIELD_DATA(Char,    char);
								READ_FIELD_DATA(Byte,    uint8);
								READ_FIELD_DATA(Short,   int16);
								READ_FIELD_DATA(Long,    int64);
								READ_FIELD_DATA(UShort,  uint16);
								READ_FIELD_DATA(UInt,    uint);
								READ_FIELD_DATA(ULong,   uint64);
								READ_FIELD_DATA(Vector2, glm::vec2);
								READ_FIELD_DATA(Vector3, glm::vec3);
								READ_FIELD_DATA(Vector4, glm::vec4);
								READ_FIELD_DATA(Entity,  UUID);
							}
						}
					}
				}
			}
		}
		return true;
	}
}