#include "tagepch.h"
#include "EndlessGrid.h"
#include <GLAD/glad.h>
#include "TAGE/Renderer/Shader/ShaderLibrary.h"

namespace TAGE::Renderer
{
	EndlessGrid::EndlessGrid()
	{
		InitGridBuffers();
	}

	EndlessGrid::~EndlessGrid()
	{
		glDeleteVertexArrays(1, &_VAO);
		glDeleteBuffers(1, &_VBO);
	}

	void EndlessGrid::Render(const MEM::Ref<Camera>& camera)
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		MEM::Ref<Shader> shader = ShaderLibrary::Get("GridShader");
		shader->Use();

		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 proj = camera->GetProjectionMatrix();
		glm::mat4 invViewProj = glm::inverse(proj * view);

		shader->SetUniform("u_View", view);
		shader->SetUniform("u_Projection", proj);
		shader->SetUniform("u_InverseViewProjection", invViewProj);
		shader->SetUniform("u_CameraPosition", camera->GetPosition());
		shader->SetUniform("u_ViewportSize", glm::vec2(1280, 720));

		shader->SetUniform("u_Near", NEAR_CLIP);
		shader->SetUniform("u_Far",  FAR_CLIP);
		shader->SetUniform("u_GridSize", DEBUG_RENDERER_GRID_SIZE);
		
		glBindVertexArray(_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
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

		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);

		glBindVertexArray(_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindVertexArray(0);
	}
}