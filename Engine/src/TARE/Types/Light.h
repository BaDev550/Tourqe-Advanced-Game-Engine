#pragma once
#include <glm/glm.hpp>
#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 1000
#define MAX_SPOT_LIGHTS 1024

struct DirectionalLight {
	glm::vec3 Direction;
	glm::vec3 Color;
	float Intensity;
	bool CastsShadows;

	DirectionalLight() : Direction(0.0f, -1.0f, 0.0f), Color(1.0f), Intensity(1.0f), CastsShadows(false) {}
};

struct PointLight {
	glm::vec3 Position;
	float Intensity;
	glm::vec3 Color;
	float Radius;
	float Falloff;
	bool CastsShadows;
	char padding[3]{ 0, 0, 0 };

	PointLight() : Position(0.0f), Color(1.0f), Intensity(1.0f), Radius(1.0f), Falloff(1.0f), CastsShadows(false) {}
};

struct SpotLight {
	glm::vec3 Position;
	float Intensity;
	glm::vec3 Direction;
	float Range;
	glm::vec3 Color;
	float Falloff;
	bool CastsShadows;
	char padding[3]{ 0, 0, 0 };

	SpotLight() : Position(0.0f), Direction(0.0f, -1.0f, 0.0f), Color(1.0f), Intensity(1.0f), Range(1.0f), Falloff(1.0f), CastsShadows(false) {}
};

struct UniformBufferDirectionalLight
{
	glm::vec3 Direction;
	glm::vec3 Color;
	float Intensity;
};

struct UniformBufferPointLights
{
	uint Count{ 0 };
	glm::vec3 Padding{ 0.0f };
	PointLight PointLights[MAX_POINT_LIGHTS]{};
};

struct UniformBufferSpotLights
{
	uint Count{ 0 };
	glm::vec3 Padding{ 0.0f };
	SpotLight SpotLights[MAX_SPOT_LIGHTS]{};
};