#pragma once

#include "TARE/API/RenderContext.h"

struct GLFWwindow;
namespace TARE::OpenGL {
	class OpenGL_RenderContext : public RenderContext
	{
	public:
		OpenGL_RenderContext(GLFWwindow* handle);
		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void* GetDevice() { return nullptr; };
	private:
		GLFWwindow* _Handle;
	};
}
