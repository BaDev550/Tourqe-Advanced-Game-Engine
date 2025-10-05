#include "tagepch.h"
#include "Grid.h"
#include <GLAD/glad.h>
#include "TARE/Shader/ShaderLibrary.h"
#include "TARE/Buffers/Buffers.h"

namespace TARE
{
	Grid::Grid() {}

	void Grid::Create() {
		InitGridBuffers();
	}

	void Grid::Draw() {
		RenderCommand::DepthTest(false);
		TARE::TARE3D::GetShaderLibrary().GetShader("Grid")->Use();
		RenderCommand::DrawVertex(_VAO);
		RenderCommand::DepthTest(true);
	}

	void Grid::InitGridBuffers()
	{
		float vertices[] = {
			1.0f,  1.0f,  0.0f,
		   -1.0f, -1.0f,  0.0f,
		   -1.0f,  1.0f,  0.0f,
		   -1.0f, -1.0f,  0.0f,
			1.0f,  1.0f,  0.0f,
			1.0f, -1.0f,  0.0f
		};

		TAGE::MEM::Ref<VertexBufferObject> _VBO;
		_VBO = VertexBufferObject::Create(vertices, sizeof(vertices));
		BufferLayout layout = {
			{ ShaderDataType::VEC3, "aPos" }
		};
		_VAO = VertexArrayObject::Create();
		_VBO->SetLayout(layout);
		_VAO->AddVertexBuffer(_VBO);
		_VAO->SetCount(sizeof(vertices));
	}
}