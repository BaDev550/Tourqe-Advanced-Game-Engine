#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/matrix4x4.h>
#include "TAGE/Utilities/Logger.h"

namespace DirectX {
	struct XMFLOAT2; // vec2
	struct XMFLOAT3; // vec3
	struct XMFLOAT4; // vec4
	struct XMFLOAT4X3; // mat3
	struct XMFLOAT4X4; // mat4
}

namespace TAGE::Math {
	glm::vec3 Scale(const glm::vec3& v, float desiredLength);
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
	glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);

	inline glm::vec2 DirectXToOpenGL(DirectX::XMFLOAT2 vec2);
	inline glm::vec3 DirectXToOpenGL(DirectX::XMFLOAT3 vec3);
	inline glm::vec4 DirectXToOpenGL(DirectX::XMFLOAT4 vec4);
	inline glm::mat3 DirectXToOpenGL(DirectX::XMFLOAT4X3 mat3);
	inline glm::mat4 DirectXToOpenGL(DirectX::XMFLOAT4X4 mat4);

	inline DirectX::XMFLOAT2 OpenGLToDirectX(glm::vec2 vec2);
	inline DirectX::XMFLOAT3 OpenGLToDirectX(glm::vec3 vec3);
	inline DirectX::XMFLOAT4 OpenGLToDirectX(glm::vec4 vec4);
	inline DirectX::XMFLOAT4X3 OpenGLToDirectX(glm::mat3 mat3);
	inline DirectX::XMFLOAT4X4 OpenGLToDirectX(glm::mat4 mat4);
}