#include "tagepch.h"
#include "OpenGL_RenderAPI.h"
#include <GLAD/glad.h>

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
		glDrawArrays(GL_TRIANGLES, 0, VAO->GetCount());
	}

	void OpenGL_RenderAPI::DrawLine(const TAGE::MEM::Ref<VertexArrayObject>& VAO, uint vertexCount)
	{
		if (vertexCount == 0)
			return;

		VAO->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
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
	void OpenGL_RenderAPI::SetCullingMode(CullMode mode)
	{
		switch (mode) {
		case CullMode::BACK:
			glCullFace(GL_BACK);
			break;
		case CullMode::FRONT:
			glCullFace(GL_FRONT);
			break;
		case CullMode::NONE:
			glDisable(GL_CULL_FACE);
			break;
		default:
			LOG_ERROR("Unknown culling mode: {}", static_cast<int>(mode));
			break;
		}
	}
	void OpenGL_RenderAPI::ToggleStencilFunction(StencilMode mode)
	{
		switch (mode)
		{
		case StencilMode::DISABLE:
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
		case StencilMode::ENABLE:
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		case StencilMode::ENABLE_FOR_READING:
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
		default:
			break;
		}
	}
}