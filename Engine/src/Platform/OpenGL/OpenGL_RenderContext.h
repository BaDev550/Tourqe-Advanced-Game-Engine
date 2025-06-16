#pragma once

#include "TARE/Common/RenderContext.h"

struct GLFWwindow;
namespace TARE {
	class OpenGL_RenderContext : public RenderContext
	{
	public:
		OpenGL_RenderContext(GLFWwindow* handle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* _Handle;
	};
}