#include "tagepch.h"
#include "Camera.h"

namespace TAGE::Renderer
{
	void Camera::Update()
	{
		_rotation.x = glm::clamp(_rotation.x, _minPitch, _maxPitch);

		_forward.x = cosf(_rotation.x) * sinf(_rotation.y);
		_forward.y = sinf(_rotation.x);
		_forward.z = -cosf(_rotation.x) * cosf(_rotation.y);
		_forward = glm::normalize(_forward);

		_right = glm::normalize(glm::cross(_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
		_up = glm::normalize(glm::cross(_right, _forward));

		_forwardXZ = glm::normalize(glm::vec3(_forward.x, 0.0f, _forward.z));

		_viewMatrix = glm::lookAt(_position, _position + _forward, _up);
		_inverseViewMatrix = glm::inverse(_viewMatrix);
		_projectionMatrix = glm::perspective(glm::radians(45.0f), _aspectRatio, NEAR_CLIP, FAR_CLIP);
	}

	void Camera::SetPosition(glm::vec3 position)
	{
		_position = position;
		Update();
	}

	void Camera::SetEulerRotation(glm::vec3 rotation)
	{
		_rotation = rotation;
		Update();
	}

	void Camera::Orbit(float pitchOffset, float yawOffset)
	{
		AddPitch(pitchOffset);
		AddYaw(yawOffset);
		Update();
	}

	void Camera::AddPitch(float value)
	{
		_rotation.x += value;
		_rotation.x = glm::clamp(_rotation.x, _minPitch, _maxPitch);
		Update();
	}

	void Camera::AddYaw(float value)
	{
		_rotation.y += value;
		Update();
	}

	void Camera::AddHeight(float value)
	{
		_position.y += value;
		Update();
	}

	void Camera::SetMinPitch(float value) { _minPitch = value; }
	void Camera::SetMaxPitch(float value) { _maxPitch = value; }

	const glm::mat4& Camera::GetViewMatrix() const { return _viewMatrix; }
	const glm::mat4& Camera::GetProjectionMatrix() const { return _projectionMatrix; }
	const glm::mat4& Camera::GetViewProjectionMatrix() const { return _projectionMatrix * _viewMatrix; }
	const glm::mat4& Camera::GetInverseViewMatrix() const { return _inverseViewMatrix; }
	const glm::vec3& Camera::GetPosition() const { return _position; }
	const glm::vec3& Camera::GetEulerRotation() const { return _rotation; }
	const glm::vec3& Camera::GetForward() const { return _forward; }
	const glm::vec3& Camera::GetUp() const { return _up; }
	const glm::vec3& Camera::GetRight() const { return _right; }
	const glm::vec3 Camera::GetForwardXZ() const { return _forwardXZ; }
}