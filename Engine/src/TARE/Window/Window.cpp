#include "tagepch.h"
#include "Window.h"

bool g_gladInitialized = false;
bool g_glfwInitialized = false;

Window::Window(WindowProperties data)
{
	_Data = data;
	if (!g_glfwInitialized) {
		int success = glfwInit();
		assert(success);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_REFRESH_RATE, _Data.refreshRate);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		g_glfwInitialized = true;
	}

	_WindowHandle = glfwCreateWindow(_Data.width, _Data.height, _Data.title, nullptr, nullptr);
	assert(_WindowHandle);
	glfwMakeContextCurrent(_WindowHandle);
	glfwSwapInterval(_Data.vSync);

	_RenderContext = TARE::RenderContext::Create(_WindowHandle);
	_RenderContext->Init();
}

void Window::SwapBuffers()
{
	_RenderContext->SwapBuffers();
	glfwPollEvents();
}

void Window::Close()
{
	glfwDestroyWindow(_WindowHandle);
	_WindowHandle = nullptr;
	glfwTerminate();
}

bool Window::ShoudClose() const
{
    return glfwWindowShouldClose(_WindowHandle);
}

void Window::EnableCursor(bool enable)
{
	glfwSetInputMode(_WindowHandle, GLFW_CURSOR, enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}
