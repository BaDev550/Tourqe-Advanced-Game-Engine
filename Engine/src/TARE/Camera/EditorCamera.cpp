#include "tagepch.h"
#include "EditorCamera.h"
#include "TAGE/Input/Input.h"

namespace TARE {
	void EditorCamera::OnUpdate(float dt)
	{
		glm::vec3 forward = glm::normalize(GetForward());
		glm::vec3 right = glm::normalize(GetRight());
		glm::vec3 position = GetPosition();

		float speed = 5.0f;
		if (TAGE::Input::IsKeyPressed(E_KEY_LEFT_SHIFT))
			speed *= 2.5f;

		if (TAGE::Input::IsKeyPressed(E_KEY_W))
			position += forward * speed * dt;
		if (TAGE::Input::IsKeyPressed(E_KEY_S))
			position -= forward * speed * dt;
		if (TAGE::Input::IsKeyPressed(E_KEY_A))
			position -= right * speed * dt;
		if (TAGE::Input::IsKeyPressed(E_KEY_D))
			position += right * speed * dt;
		if (TAGE::Input::IsKeyPressed(E_KEY_E))
			position.y += speed * dt;
		if (TAGE::Input::IsKeyPressed(E_KEY_Q))
			position.y -= speed * dt;

		SetPosition(position);

		double xpos, ypos;
		xpos = TAGE::Input::GetMousePosition().x;
		ypos = TAGE::Input::GetMousePosition().y;

		if (IsFirstMouse()) {
			_lastX = xpos;
			_lastY = ypos;
			SetFirstMouse(false);
		}

		float xoffset = static_cast<float>(xpos - _lastX);
		float yoffset = static_cast<float>(_lastY - ypos);

		_lastX = xpos;
		_lastY = ypos;

		xoffset *= 1.0f * dt;
		yoffset *= 1.0f * dt;

		Orbit(yoffset, xoffset);
		CalculateCameraMatrixes();
	}

	void EditorCamera::OnEvent(TAGE::Event& event)
	{
	}
}