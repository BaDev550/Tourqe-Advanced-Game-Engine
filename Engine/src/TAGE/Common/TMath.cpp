#include "tagepch.h"
#include "TMath.h"

#include <DirectXMath.h>

glm::vec3 TAGE::Math::Scale(const glm::vec3& v, float desiredLength)
{
	float mag = glm::length(v);
	if (glm::epsilonEqual(mag, 0.0f, glm::epsilon<float>()))
		return glm::vec3(0.0f);

	return v * desiredLength / mag;
}

bool TAGE::Math::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
{
	using namespace glm;
	using T = float;

	mat4 LocalMatrix(transform);

	if (epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), epsilon<T>()))
		return false;

	ASSERT_NOMSG(epsilonEqual(LocalMatrix[3][3], static_cast<T>(1), static_cast<T>(0.00001)));
	ASSERT_NOMSG(
		epsilonEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) &&
		epsilonEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) &&
		epsilonEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())
	);

	translation = vec3(LocalMatrix[3]);
	LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

	vec3 Row[3];
	for (length_t i = 0; i < 3; ++i)
		for (length_t j = 0; j < 3; ++j)
			Row[i][j] = LocalMatrix[i][j];

	scale.x = length(Row[0]);
	Row[0] = Scale(Row[0], static_cast<T>(1));

	scale.y = length(Row[1]);
	Row[1] = Scale(Row[1], static_cast<T>(1));

	scale.z = length(Row[2]);
	Row[2] = Scale(Row[2], static_cast<T>(1));
#if _DEBUG
	vec3 Pdum3 = cross(Row[1], Row[2]);
	ASSERT_NOMSG(dot(Row[0], Pdum3) >= static_cast<T>(0));
#endif
	int i, j, k = 0;
	T root, trace = Row[0].x + Row[1].y + Row[2].z;
	if (trace > static_cast<T>(0))
	{
		root = sqrt(trace + static_cast<T>(1));
		rotation.w = static_cast<T>(0.5) * root;
		root = static_cast<T>(0.5) / root;
		rotation.x = root * (Row[1].z - Row[2].y);
		rotation.y = root * (Row[2].x - Row[0].z);
		rotation.z = root * (Row[0].y - Row[1].x);
	}
	else
	{
		static int Next[3] = { 1, 2, 0 };
		i = 0;
		if (Row[1].y > Row[0].x) i = 1;
		if (Row[2].z > Row[i][i]) i = 2;
		j = Next[i];
		k = Next[j];

		root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<T>(1.0));

		rotation[i] = static_cast<T>(0.5) * root;
		root = static_cast<T>(0.5) / root;
		rotation[j] = root * (Row[i][j] + Row[j][i]);
		rotation[k] = root * (Row[i][k] + Row[k][i]);
		rotation.w = root * (Row[j][k] - Row[k][j]);
	}

	return true;
}

glm::mat4 TAGE::Math::ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

glm::vec2 TAGE::Math::DirectXToOpenGL(DirectX::XMFLOAT2 vec2) { return glm::vec2(vec2.x, vec2.y); }
glm::vec3 TAGE::Math::DirectXToOpenGL(DirectX::XMFLOAT3 vec3) { return glm::vec3(vec3.x, vec3.y, vec3.z); }
glm::vec4 TAGE::Math::DirectXToOpenGL(DirectX::XMFLOAT4 vec4) { return glm::vec4(vec4.x, vec4.y, vec4.z, vec4.w); }
glm::mat3 TAGE::Math::DirectXToOpenGL(DirectX::XMFLOAT4X3 mat3) { return glm::make_mat3(&mat3._11); }
glm::mat4 TAGE::Math::DirectXToOpenGL(DirectX::XMFLOAT4X4 mat4) { return glm::make_mat4(&mat4._11); }

DirectX::XMFLOAT2 TAGE::Math::OpenGLToDirectX(glm::vec2 vec2) { return DirectX::XMFLOAT2(vec2.x, vec2.x); }
DirectX::XMFLOAT3 TAGE::Math::OpenGLToDirectX(glm::vec3 vec3) { return DirectX::XMFLOAT3(vec3.x, vec3.y, vec3.z); }
DirectX::XMFLOAT4 TAGE::Math::OpenGLToDirectX(glm::vec4 vec4) { return DirectX::XMFLOAT4(vec4.x, vec4.y, vec4.z, vec4.w); }
 
DirectX::XMFLOAT4X3 TAGE::Math::OpenGLToDirectX(glm::mat3 mat3)
{
	DirectX::XMFLOAT4X3 out;
	memcpy(&out, glm::value_ptr(mat3), sizeof(float) * 12);
	return out;
}

DirectX::XMFLOAT4X4 TAGE::Math::OpenGLToDirectX(glm::mat4 mat4)
{
	DirectX::XMFLOAT4X4 out;
	memcpy(&out, glm::value_ptr(mat4), sizeof(float) * 16);
	return out;
}
