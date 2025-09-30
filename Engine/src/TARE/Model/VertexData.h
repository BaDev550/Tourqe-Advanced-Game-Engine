#pragma once
#include "glm/glm.hpp"
#define MAX_BONE_INFLUENCES 4

struct VertexData {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct SkinnedVertexData : public VertexData {
	int BoneIDs[MAX_BONE_INFLUENCES];
	float BoneWeights[MAX_BONE_INFLUENCES];
};