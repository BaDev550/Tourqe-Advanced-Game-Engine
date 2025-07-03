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
		_GridShader = ShaderLibrary::Add("GridShader", "../Engine/shaders/grid_Vertex", "../Engine/shaders/grid_Fragment");
		InitGridBuffers();
	}

	void EndlessGrid::Render(glm::mat4& view, glm::mat4& proj, glm::vec3& pos)
	{
		RenderCommand::Enable(BLEND_TEST);
		RenderCommand::Disable(DEPTH_TEST);
		RenderCommand::Disable(CULL_TEST);
		glm::mat4 invViewProj = glm::inverse(proj * view);

		_GridShader->Use();
		_GridShader->SetUniform("u_View", view);
		_GridShader->SetUniform("u_Projection", proj);
		_GridShader->SetUniform("u_InverseViewProjection", invViewProj);
		_GridShader->SetUniform("u_CameraPosition", pos);
		_GridShader->SetUniform("u_ViewportSize", glm::vec2(1280, 720));

		_GridShader->SetUniform("u_Near", NEAR_CLIP);
		_GridShader->SetUniform("u_Far",  FAR_CLIP);
		_GridShader->SetUniform("u_GridSize", DEBUG_RENDERER_GRID_SIZE);
		
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