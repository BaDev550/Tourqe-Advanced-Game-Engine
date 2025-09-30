#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "TDefines.h"
#include "TEnums.h"
#include "TAGE/Utilities/Memory.h"

#define GLM_ENABLE_EXPERIMENTAL
#define TAGE_ENABLE_GLM_VERTEX_DATA
#include <glm/gtx/quaternion.hpp>

struct RaycastLine {
	glm::vec3 from;
	glm::vec3 to;
	glm::vec3 color;
	RayDrawType drawType;
	float time;
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
	uint DepthArrayLayers = 1;

	bool SwapChainTarget = false;
	FramebufferSpecification(FramebufferAttachmentSpecification attachments, uint samples, uint width, uint height, uint depthArrayLayers = 1) 
		: Attachments(attachments), Samples(samples), Width(width), Height(height), DepthArrayLayers(depthArrayLayers) {}
};