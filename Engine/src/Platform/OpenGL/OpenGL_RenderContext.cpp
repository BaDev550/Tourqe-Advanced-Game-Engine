#include "tagepch.h"
#include "OpenGL_RenderContext.h"
#include <GLAD/glad.h>
#include "GLFW/glfw3.h"

namespace TARE {
	OpenGL_RenderContext::OpenGL_RenderContext(GLFWwindow* handle)
		: _Handle(handle)
	{
		ASSERT_NOMSG(_Handle);
	}

	void gladErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;
		LOG_ERROR("OpenGL Error: {}", message);
	}

	void OpenGL_RenderContext::Init()
	{
		int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!success)
		{
			LOG_ERROR("Failed to initialize GLAD");
			glfwDestroyWindow(_Handle);
			glfwTerminate();
			return;
		}
		else {
			LOG_INFO("GLAD initialized successfully");
			LOG_INFO("OpenGL Version: {}.{}", GLVersion.major, GLVersion.minor);
			LOG_INFO("OpenGL Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
			LOG_INFO("OpenGL Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
			LOG_INFO("OpenGL Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		}

		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);    
		glDepthMask(GL_TRUE);
		glEnable(GL_MULTISAMPLE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(gladErrorCallback, nullptr);
#endif
	}

	void OpenGL_RenderContext::SwapBuffers()
	{
		glfwSwapBuffers(_Handle);
	}
}