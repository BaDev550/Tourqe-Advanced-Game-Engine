#include "tagepch.h"
#include "SceneSerializer.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/World/Components/RenderComponents.h"
#include <yaml-cpp/yaml.h>

namespace YAML {
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
}
namespace TAGE {
	SceneSerializer::SceneSerializer(const MEM::Ref<Scene>& scene) : _Scene(scene) {}

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
		out << YAML::Key << "Entity" << YAML::Value << (uint64)entity.GetHandle();

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

		if (entity.HasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transform.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshComponent>()) {
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;
			auto& mesh = entity.GetComponent<MeshComponent>();
			out << YAML::Key << "MeshPath" << YAML::Value << mesh.Handle->GetFilePath();
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
			out << YAML::Key << "Type" << YAML::Value << (int)light.Handle.type;
			out << YAML::Key << "Color" << YAML::Value << light.Handle.color;
			out << YAML::Key << "Range" << YAML::Value << light.Handle.range;
			out << YAML::Key << "Intensity" << YAML::Value << light.Handle.intensity;
			if (light.Handle.type == LightType::SPOT) {
				out << YAML::Key << "InnerCone" << YAML::Value << light.Handle.innerCone;
				out << YAML::Key << "OuterCone" << YAML::Value << light.Handle.outerCone;
			}

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SkyboxComponent>()) {
			out << YAML::Key << "SkyboxComponent";
			out << YAML::BeginMap;
			auto& skybox = entity.GetComponent<SkyboxComponent>();
			out << YAML::Key << "TexturePath" << YAML::Value << skybox.Handle->GetTexture()->GetPath();
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
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

	void SceneSerializer::SerializeBinary(const std::string& filepath)
	{
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
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
				uint64 entityID = entity["Entity"].as<uint64>();

				std::string name;
				auto IdentityComponent = entity["IdentityComponent"];
				if (IdentityComponent)
					name = IdentityComponent["Name"].as<std::string>();

				LOG_TRACE("Deserializing entity with ID: {}, Name: {}", entityID, name);

				Entity& deserializedEntity = _Scene->CreateEntity(name);
				
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent) {
					auto& transform = deserializedEntity.GetComponent<TransformComponent>();
					transform.Position = transformComponent["Position"].as<glm::vec3>();
					transform.Rotation = transformComponent["Rotation"].as<glm::quat>();
					transform.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent) {
					auto& meshPath = meshComponent["MeshPath"].as<std::string>();
					auto& mesh = deserializedEntity.AddComponent<MeshComponent>(meshPath);
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					auto& cameraData = cameraComponent["Camera"];
					auto& camera = deserializedEntity.AddComponent<CameraComponent>();
					camera.Handle->SetAspectRatio(cameraData["AspectRatio"].as<float>());
					camera.Handle->SetFOV(cameraData["FOV"].as<float>());
					camera.Handle->SetNearClip(cameraData["NearClip"].as<float>());
					camera.Handle->SetFarClip(cameraData["FarClip"].as<float>());
					camera.IsActive = cameraComponent["IsActive"].as<bool>();
				}

				auto lightComponent = entity["LightComponent"];
				if (lightComponent) {
					LightType type = (LightType)lightComponent["Type"].as<int>();
					glm::vec3 color = lightComponent["Color"].as<glm::vec3>();
					float range = lightComponent["Range"].as<float>();
					float intensity = lightComponent["Intensity"].as<float>();
					float innerCone = 0.0f, outerCone = 0.0f;
					if (type == LightType::SPOT) {
						innerCone = lightComponent["InnerCone"].as<float>();
						outerCone = lightComponent["OuterCone"].as<float>();
					}
					auto& light = deserializedEntity.AddComponent<LightComponent>(Light(type, {}, {}, color, range, intensity, innerCone, outerCone));
				}

				auto skyboxComponent = entity["SkyboxComponent"];
				if (skyboxComponent) {
					auto& texturePath = skyboxComponent["TexturePath"].as<std::string>();
					auto& skybox = deserializedEntity.AddComponent<SkyboxComponent>(texturePath);
				}
			}
		}
		return true;
	}

	bool SceneSerializer::DeserializeBinary(const std::string& filepath)
	{
		return false;
	}
}