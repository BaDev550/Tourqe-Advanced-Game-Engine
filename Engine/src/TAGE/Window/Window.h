#pragma once
#include "TAGE/Common/TDefines.h"

#include <iostream>
#include "TARE/Common/RenderContext.h"
#include "TAGE/Events/Event.h"
#include "TAGE/Utilities/Memory.h"
#include <GLFW/glfw3.h>

namespace TAGE {
	using EventCallbackFn = std::function<void(Event&)>;

	enum class WindowMode {
		WINDOWED,
		FULLSCREEN
	};

	struct SWindow {
		const char* Title = "NOTSET";
		int Width = 800;
		int Height = 800;
		bool VSync = true;
		bool Resizable = true;
		bool Focused = true;
		bool Visible = true;
		WindowMode Mode = WindowMode::WINDOWED;
		EventCallbackFn EventCallback;

		SWindow(const char* title = "NOTSET", int width = 800, int height = 800, bool vsync = true) :
			Title(title), Width(width), Height(height), VSync(vsync) {};
	};

	class Window
	{
	public:
		Window(SWindow properties = SWindow(), WindowMode mode = WindowMode::WINDOWED);
		~Window() = default;
		void Create(SWindow properties);
		void Destroy();

		void Resize(int width, int height);
		void SetTitle(const char* title);
		void SetVSync(bool enabled);
		void SetWindowMode(WindowMode mode);
		void ToggleFullscreen();
		void ForceClose();
		void ToggleCursor(bool enabled);

		bool ShouldClose() const;
		void SwapBuffers();

		bool IsVSync() const { return _Properties.VSync; }
		bool IsFocused() const { return _Properties.Focused; }
		bool IsVisible() const { return _Properties.Visible; }
		bool IsMinimized() const { return glfwGetWindowAttrib(_Window, GLFW_ICONIFIED); }
		GLFWwindow* GetGLFWWindow() const { return _Window; }

		int GetWidth() const { return _Properties.Width; }
		int GetHeight() const { return _Properties.Height; }
		std::string GetTitle() const { return glfwGetWindowTitle(_Window); }

		void SetEventCallback(const EventCallbackFn& callback) { _Properties.EventCallback = callback; }
	private:
		GLFWwindow* _Window = nullptr;
		SWindow _Properties;
		bool _ForceClose = false;

		GLFWmonitor* _Monitor;
		const GLFWvidmode* _VideoMode = nullptr;

		MEM::Scope<TARE::RenderContext> _RenderContext;
	};
}