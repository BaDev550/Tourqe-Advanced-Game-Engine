#pragma once
#include <cstdint>
#include <assert.h>
#include "GLAD/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "TARE/Common/RenderContext.h"

struct WindowProperties {
	uint16_t width;
	uint16_t height;
	uint8_t refreshRate = 60;
	bool vSync = true;
	const char* title;

	WindowProperties(uint16_t w = 800, uint16_t h = 600, const char* t = "Window")
		: width(w), height(h), title(t) {
	}
};

class Window
{
public:
	Window(WindowProperties data = {});

	void SwapBuffers();
	void Close();
	bool ShoudClose() const;
	void EnableCursor(bool enable = true);

	int GetWidth() const { return _Data.width; }
	int GetHeight() const { return _Data.height; }

	GLFWwindow* GetWindowHandle() const { return _WindowHandle; }
private:
	WindowProperties _Data;
	GLFWwindow* _WindowHandle = nullptr;
	TAGE::MEM::Ref<TARE::RenderContext> _RenderContext;
};

