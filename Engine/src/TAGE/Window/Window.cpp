#include "tagepch.h"
#include "Window.h"
#include "TAGE/Utilities/Logger.h"
#include "TAGE/Events/ApplicationEvents.h"
#include "TAGE/Events/InputEvents.h"

#include "Platform/OpenGL/OpenGL_RenderContext.h"

namespace TAGE
{
	void Window::Create(SWindow properties)
	{
		glfwSetErrorCallback([](int error, const char* description) { LOG_ERROR("GLFW Error ({}): {}\n", std::to_string(error), description); });

		_Properties.Title = properties.Title;

		int success = glfwInit();
		ASSERT(success, "Failed to initialize GLFW");
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		_Monitor = glfwGetPrimaryMonitor();
		ASSERT(_Monitor, "Failed to get primary monitor");
		_VideoMode = glfwGetVideoMode(_Monitor);

		glfwWindowHint(GLFW_RED_BITS, _VideoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, _VideoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, _VideoMode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, _VideoMode->refreshRate);

		if (properties.Mode == WindowMode::FULLSCREEN)
		{
			_Properties.Width = _VideoMode->width;
			_Properties.Height = _VideoMode->height;
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
			_Window = glfwCreateWindow(_Properties.Width, _Properties.Height, _Properties.Title, _Monitor, nullptr);
		}
		else {
			_Properties.Width = properties.Width;
			_Properties.Height = properties.Height;
			glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
			glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
			_Window = glfwCreateWindow(_Properties.Width, _Properties.Height, _Properties.Title, nullptr, nullptr);
		}
		ASSERT(_Window, "Failed to create GLFW window");
		glfwMakeContextCurrent(_Window);
		glfwSetWindowUserPointer(_Window, &_Properties);
		SetVSync(properties.VSync);

		glfwSetWindowSizeCallback(_Window, [](GLFWwindow* window, int width, int height) {
			SWindow& data = *(SWindow*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(_Window, [](GLFWwindow* window) {
			SWindow& data = *(SWindow*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
			});

		glfwSetKeyCallback(_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			SWindow& data = *(SWindow*)glfwGetWindowUserPointer(window);
			switch (action)
			{
			case GLFW_PRESS: {
				KeyPressedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent event(key);
				data.EventCallback(event);
				break;
			}
			default:
				std::cout << "Incorrect Key action!" << std::endl;
				break;
			}
			});

		glfwSetMouseButtonCallback(_Window, [](GLFWwindow* window, int button, int action, int mods) {
			SWindow& data = *(SWindow*)glfwGetWindowUserPointer(window);
			switch (action)
			{
			case GLFW_PRESS: {
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			default:
				std::cout << "Incorrect Mouse action!" << std::endl;
				break;
			}
			});

		glfwSetScrollCallback(_Window, [](GLFWwindow* window, double x, double y) {
			SWindow& data = *(SWindow*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)x, (float)y);
			data.EventCallback(event);
			});

		glfwSetCursorPosCallback(_Window, [](GLFWwindow* window, double xPos, double yPos) {
			SWindow& data = *(SWindow*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
			});

		_RenderContext = MEM::MakeScope<TARE::OpenGL_RenderContext>(_Window);
		_RenderContext->Init();
	}

	Window::Window(SWindow properties, WindowMode mode)
	{
		properties.Mode = mode;
		Create(properties);
	}

	void Window::Destroy()
	{
		if (_Window)
		{
			glfwDestroyWindow(_Window);
			_Window = nullptr;
		}
		glfwTerminate();
		LOG_INFO("Window destroyed successfully");
	}

	void Window::Resize(int width, int height)
	{
		if (width <= 0 || height <= 0)
		{
			LOG_ERROR("Invalid window size: {}x{}", width, height);
			return;
		}
		_Properties.Width = width;
		_Properties.Height = height;
		glfwSetWindowSize(_Window, width, height);
		LOG_INFO("Window resized to {}x{}", width, height);
	}

	void Window::SetTitle(const char* title)
	{
		if (!title || title[0] == '\0')
		{
			LOG_ERROR("Invalid title provided");
			return;
		}
		_Properties.Title = title;
		glfwSetWindowTitle(_Window, title);
		LOG_INFO("Window title set to: {}", title);
	}

	void Window::SetVSync(bool enabled)
	{
		_Properties.VSync = enabled;
		if (enabled)
		{
			glfwSwapInterval(1);
			LOG_INFO("VSync enabled");
		}
		else
		{
			glfwSwapInterval(0);
			LOG_INFO("VSync disabled");
		}
	}

	void Window::SetWindowMode(WindowMode mode)
	{
		if (mode == WindowMode::FULLSCREEN)
		{
			_Properties.Mode = WindowMode::FULLSCREEN;
			glfwSetWindowMonitor(_Window, _Monitor, 0, 0, _VideoMode->width, _VideoMode->height, _VideoMode->refreshRate);
			LOG_INFO("Window set to fullscreen mode");
		}
		else
		{
			_Properties.Mode = WindowMode::WINDOWED;
			glfwSetWindowMonitor(_Window, nullptr, 100, 100, _Properties.Width, _Properties.Height, 0);
			LOG_INFO("Window set to windowed mode");
		}
	}

	void Window::ToggleFullscreen()
	{
		if (_Properties.Mode == WindowMode::FULLSCREEN)
		{
			SetWindowMode(WindowMode::WINDOWED);
		}
		else
		{
			SetWindowMode(WindowMode::FULLSCREEN);
		}
	}

	void Window::ForceClose()
	{
		_ForceClose = true;
	}

	void Window::ToggleCursor(bool enabled)
	{
		if (enabled)
		{
			glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			LOG_INFO("Cursor enabled");
		}
		else
		{
			glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			LOG_INFO("Cursor disabled");
		}
		_Properties.Focused = glfwGetWindowAttrib(_Window, GLFW_FOCUSED);
	}

	bool Window::ShouldClose() const
	{
		return (glfwWindowShouldClose(_Window) || _ForceClose);
	}

	void Window::SwapBuffers()
	{
		_RenderContext->SwapBuffers();
		glfwPollEvents();

		_Properties.Focused = glfwGetWindowAttrib(_Window, GLFW_FOCUSED);
		_Properties.Visible = glfwGetWindowAttrib(_Window, GLFW_VISIBLE);
	}

}