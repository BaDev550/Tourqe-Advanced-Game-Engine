#pragma once
#include "glm/glm.hpp"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Utilities/Memory.h"
#include "TARE/Shadow/ShadowMap.h"

struct Light {
	LightType type;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 color = glm::vec3(1.0f);
	float intensity = 1.0f;

	float range = 10.0f;
	float innerCone = glm::radians(15.0f);
	float outerCone = glm::radians(30.0f);
	glm::mat4 lightSpaceMatrix;

	bool castShadow = false;
	TAGE::MEM::Ref<TARE::ShadowMap> shadowMap;

	Light(
		LightType type = LightType::POINT,
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 dir = glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3 col = glm::vec3(1.0f),
		float inten = 1.0f,
		float rng = 10.0f,
		float inner = glm::radians(15.0f),
		float outer = glm::radians(30.0f),
		bool castshadow = true
	)
		: castShadow(castshadow),
		type(type),
		position(pos),
		direction(dir),
		color(col),
		intensity(inten),
		range(rng),
		innerCone(inner),
		outerCone(outer),
		shadowMap(TAGE::MEM::MakeRef<TARE::ShadowMap>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION))
	{
	}

	void SetShadowMap(const TAGE::MEM::Ref<TARE::ShadowMap>& map) { shadowMap = map; }
};