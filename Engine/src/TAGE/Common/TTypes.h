#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "TDefines.h"
#include "TEnums.h"

#define GLM_ENABLE_EXPERIMENTAL
#define TAGE_ENABLE_GLM_VERTEX_DATA
#include <glm/gtx/quaternion.hpp>

#ifdef TAGE_ENABLE_GLM_VERTEX_DATA
#pragma pack(push, 1)
struct VertexData {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};
#pragma pack(pop)
#else
#pragma pack(push, 1)
struct VertexData {
	int16 pos[3];
	int8 normal[3];
	uint16 uv[2];
	int8 tangent[3];
	int8 bitangent[3];
};
#pragma pack(pop)
#endif

struct SkinedVertexData {
	VertexData Vertex;
	uint BoneIDs[MAX_BONE_INFLUENCES];
	float BoneWeights[MAX_BONE_INFLUENCES];

	SkinedVertexData() {
		for (int i = 0; i < MAX_BONE_INFLUENCES; ++i) {
			BoneIDs[i] = 0;
			BoneWeights[i] = 0.0f;
		}
	}
};

struct BoundingBox {
	glm::vec3 Min;
	glm::vec3 Max;
};

struct Bone {
	std::string Name;
	int ParentIndex = -1;
	glm::mat4 OffsetMatrix = glm::mat4(1.0f);
	glm::mat4 LocalTransform = glm::mat4(1.0f);
	glm::mat4 GlobalTransform = glm::mat4(1.0f);
	std::vector<int> ChildrenIndices;
};

struct Keyframe {
	float Time;
	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;
};

struct BoneAnimation {
	std::string BoneName;
	std::vector<Keyframe> PositionKeys;
	std::vector<Keyframe> RotationKeys;
	std::vector<Keyframe> ScaleKeys;

	inline int FindKeyIndex(float time, const std::vector<Keyframe>& keys) const {
		for (size_t i = 0; i < keys.size() - 1; ++i) {
			if (time < keys[i + 1].Time) return static_cast<int>(i);
		}
		return static_cast<int>(keys.size() - 2);
	}

	inline glm::vec3 InterpolateVec3(float time, const std::vector<Keyframe>& keys, bool isScale = false) const {
		if (keys.empty()) return isScale ? glm::vec3(1.0f) : glm::vec3(0.0f);
		if (keys.size() == 1) return isScale ? keys[0].Scale : keys[0].Position;

		int i = FindKeyIndex(time, keys);
		float t = (time - keys[i].Time) / (keys[i + 1].Time - keys[i].Time);

		return isScale
			? glm::mix(keys[i].Scale, keys[i + 1].Scale, t)
			: glm::mix(keys[i].Position, keys[i + 1].Position, t);
	}

	inline glm::quat InterpolateRot(float time, const std::vector<Keyframe>& keys) const {
		if (keys.empty()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		if (keys.size() == 1) return keys[0].Rotation;

		int i = FindKeyIndex(time, keys);
		float t = (time - keys[i].Time) / (keys[i + 1].Time - keys[i].Time);
		return glm::slerp(keys[i].Rotation, keys[i + 1].Rotation, t);
	}

	inline glm::mat4 Interpolate(float time) const {
		glm::vec3 pos = InterpolateVec3(time, PositionKeys, false);
		glm::quat rot = InterpolateRot(time, RotationKeys);
		glm::vec3 scale = InterpolateVec3(time, ScaleKeys, true);

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);
		glm::mat4 rotation = glm::toMat4(rot);
		glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

		return translation * rotation * scaling;
	}
};

struct Light {
	LightType type;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 color = glm::vec3(1.0f);
	float intensity = 1.0f;

	float range = 10.0f;

	float innerCone = glm::radians(15.0f);
	float outerCone = glm::radians(30.0f);

	Light(
		LightType type = LightType::POINT,
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 dir = glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3 col = glm::vec3(1.0f),
		float inten = 1.0f,
		float rng = 10.0f,
		float inner = glm::radians(15.0f),
		float outer = glm::radians(30.0f))
		: type(type), position(pos), direction(dir), color(col), intensity(inten), range(rng), innerCone(inner), outerCone(outer) {}

};

struct FramebufferTextureSpecification {
	FramebufferTextureFormat format = FramebufferTextureFormat::RGBA8;

	FramebufferTextureSpecification() = default;
	FramebufferTextureSpecification(FramebufferTextureFormat fmt) : format(fmt) { }
};

struct FramebufferAttachmentSpecification {
	std::vector<FramebufferTextureSpecification> Attachments;

	FramebufferAttachmentSpecification() = default;
	FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) : Attachments(attachments) {}
};

struct FramebufferSpecification
{
	uint Width = 0, Height = 0;
	FramebufferAttachmentSpecification Attachments;
	uint Samples = 1;

	bool SwapChainTarget = false;
	FramebufferSpecification(FramebufferAttachmentSpecification attachments, uint samples, uint width, uint height) : Attachments(attachments), Samples(samples), Width(width), Height(height) {}
};