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
}