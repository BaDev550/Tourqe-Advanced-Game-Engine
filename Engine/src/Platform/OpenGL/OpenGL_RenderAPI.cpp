#include "tagepch.h"
#include "OpenGL_RenderAPI.h"

#include "GLAD/glad.h"

namespace TARE::OpenGL {
	void OpenGL_RenderAPI::SetClearColor(const glm::vec4& color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGL_RenderAPI::Clear(uint properties)
	{
		glClear(properties);
	}

	void OpenGL_RenderAPI::SetViewport(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGL_RenderAPI::DepthTest(bool enabled)
	{
		if (enabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGL_RenderAPI::BlendTest(bool enabled)
	{
		if (enabled)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	void OpenGL_RenderAPI::DrawIndexed(const TAGE::MEM::Ref<VertexArrayObject>& VAO)
	{
		if (VAO->GetIndexBuffer() != nullptr && VAO->GetIndexBuffer()->GetCount() > 0) {
			VAO->Bind();
			glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
		}
	}

	void OpenGL_RenderAPI::DrawVertex(const TAGE::MEM::Ref<VertexArrayObject>& VAO)
	{
		VAO->Bind();
		glDrawArrays(GL_TRIANGLES, 0, VAO->GetVertexCount());
	}

	void OpenGL_RenderAPI::DrawLine(const TAGE::MEM::Ref<VertexArrayObject>& VAO, uint vertexCount)
	{
		if (vertexCount != 0) {
			VAO->Bind();
			glDrawArrays(GL_LINES, 0, vertexCount);
		}
	}
	void OpenGL_RenderAPI::DrawFullScreenQuad()
	{
		static GLuint vao = 0;
		if (vao == 0)
		{
			GLuint vbo;
			float quadVertices[] = {
				// positions    // texCoords
				-1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f, 1.0f, // top-right

				-1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f,  1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  0.0f, 1.0f  // top-left
			};
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		}
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}
}