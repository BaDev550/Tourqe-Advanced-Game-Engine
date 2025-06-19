#include "tagepch.h"
#include "OpenGL_RenderAPI.h"
#include "GLAD/glad.h"

namespace TARE {
	void OpenGL_RenderAPI::SetClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }
	void OpenGL_RenderAPI::Clear(uint properties) { glClear(static_cast<GLbitfield>(properties)); }
	void OpenGL_RenderAPI::SetViewport(int x, int y, int width, int height) { glViewport(x, y, width, height); }

	void OpenGL_RenderAPI::DrawIndexed(const TAGE::MEM::Ref<VertexArrayObject>& VAO) {
		VAO->Bind();
		glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
	}
	void OpenGL_RenderAPI::DrawVertex(const TAGE::MEM::Ref<VertexArrayObject>& VAO) { 
		VAO->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6); // To-do: implement a count to vao
	}

	void OpenGL_RenderAPI::Enable(uint properties)
	{
		glEnable(properties);
	}
	void OpenGL_RenderAPI::Disable(uint properties)
	{
		glDisable(properties);
	}
	void OpenGL_RenderAPI::BindTextureFromID(uint64 textureID, uint slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(textureID));
	}
	void OpenGL_RenderAPI::DrawFullScreenQuad()
	{
		static GLuint vao = 0;
		if (vao == 0)
		{
			float quadVertices[] = {
				// positions    // texCoords
				-1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f, 1.0f, // top-right

				-1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f,  1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  0.0f, 1.0f  // top-left
			};

			GLuint vbo;
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