#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "KeyCodes.h"
#include "MouseCodes.h"
#include <unordered_map>
#include <string>

namespace TAGE {
	class Input
	{
	public:
		static void Init(GLFWwindow* window);
		static void Update();

		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyJustPressed(KeyCode key);
		static bool IsKeyJustReleased(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonJustPressed(MouseCode button);
		static bool IsMouseButtonJustReleased(MouseCode button);

		static glm::vec2 GetMousePosition();
		static glm::vec2 GetMouseDelta();
		static float GetMouseX();
		static float GetMouseY();

		static float GetScrollDelta();

		static void BindAction(const std::string& name, KeyCode key);
		static bool IsActionPressed(const std::string& name);

	private:
		static GLFWwindow* _Window;

		static bool _KeyStates[GLFW_KEY_LAST];
		static bool _PrevKeyStates[GLFW_KEY_LAST];

		static bool _MouseButtonStates[GLFW_MOUSE_BUTTON_LAST];
		static bool _PrevMouseButtonStates[GLFW_MOUSE_BUTTON_LAST];

		static glm::vec2 _MousePos;
		static glm::vec2 _LastMousePos;
		static glm::vec2 _MouseDelta;

		static float _ScrollDelta;

		static std::unordered_map<std::string, int> _ActionMap;

		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	};
}