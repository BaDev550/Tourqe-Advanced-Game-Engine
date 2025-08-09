#include "tagepch.h"
#include "EndlessGrid.h"
#include <GLAD/glad.h>
#include "TARE/Shader/ShaderLibrary.h"
#include "TARE/Buffers/Buffers.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE
{
	EndlessGrid::EndlessGrid()
	{
		_GridShader = ShaderLibrary::Add("GridShader", "shaders/Debugger/grid_Vertex", "shaders/Debugger/grid_Fragment");
		InitGridBuffers();
	}

	void EndlessGrid::Render()
	{
		RenderCommand::Enable(BLEND_TEST);
		RenderCommand::Disable(DEPTH_TEST);
		RenderCommand::Disable(CULL_TEST);

		_GridShader->Use();
		RenderCommand::DrawVertex(_VAO);
		RenderCommand::Enable(DEPTH_TEST);
		RenderCommand::Enable(CULL_TEST);
		RenderCommand::Disable(BLEND_TEST);
	}

	void EndlessGrid::InitGridBuffers()
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