#include "tagepch.h"
#include "FreeCamera.h"
#include "TAGE/Application/Application.h"
#include "TAGE/Input/Input.h"

namespace TAGE {
	FreeCamera::FreeCamera()
	{
		_Camera = MEM::MakeRef<Renderer::Camera>(Application::Get()->GetWindow()->GetWidth(), Application::Get()->GetWindow()->GetHeight());
	}

	void FreeCamera::Update(float deltaTime)
	{
		glm::vec3 forward = glm::normalize(_Camera->GetForward());
		glm::vec3 right = glm::normalize(_Camera->GetRight());
		glm::vec3 position = _Camera->GetPosition();

		float speed = _Speed;
		if (Input::IsKeyPressed(E_KEY_LEFT_SHIFT))
			speed *= 2.5f;

		if (Input::IsKeyPressed(E_KEY_W))
			position += forward * speed * deltaTime;
		if (Input::IsKeyPressed(E_KEY_S))
			position -= forward * speed * deltaTime;
		if (Input::IsKeyPressed(E_KEY_A))
			position -= right * speed * deltaTime;
		if (Input::IsKeyPressed(E_KEY_D))
			position += right * speed * deltaTime;
		if (Input::IsKeyPressed(E_KEY_E))
			position.y += speed * deltaTime;
		if (Input::IsKeyPressed(E_KEY_Q))
			position.y -= speed * deltaTime;

		_Camera->SetPosition(position);

		double xpos, ypos;
		glfwGetCursorPos(Application::Get()->GetWindow()->GetGLFWWindow(), &xpos, &ypos);

		if (_Camera->IsFirstMouse()) {
			_lastX = xpos;
			_lastY = ypos;
			_Camera->SetFirstMouse(false);
		}

		float xoffset = static_cast<float>(xpos - _lastX);
		float yoffset = static_cast<float>(_lastY - ypos);

		_lastX = xpos;
		_lastY = ypos;

		xoffset *= 1.0f * deltaTime;
		yoffset *= 1.0f * deltaTime;

		_Camera->Orbit(yoffset, xoffset);
		_Camera->Update();
	}
}