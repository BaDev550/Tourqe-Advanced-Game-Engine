#include "tagepch.h"
#include "OpenGL_RenderAPI.h"
#include "GLAD/glad.h"

namespace TARE {
	void OpenGL_RenderAPI::SetClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }
	void OpenGL_RenderAPI::Clear(uint properties) { glClear(static_cast<GLbitfield>(properties)); }
	void OpenGL_RenderAPI::SetViewport(int x, int y, int width, int height) { glViewport(x, y, width, height); }

	void OpenGL_RenderAPI::DrawIndexed() { }
	void OpenGL_RenderAPI::DrawVertex() { }
}