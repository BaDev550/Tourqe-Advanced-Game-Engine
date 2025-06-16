#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

namespace TAGE::Renderer
{
	class Camera
	{
	public:
        Camera(float win_Width, float win_Height) {
			_aspectRatio = win_Width / win_Height;
        }

        void Update();
        void SetPosition(glm::vec3 position);
        void SetEulerRotation(glm::vec3 rotation);
        void Orbit(float pitchOffset, float yawOffset);
        void AddPitch(float value);
        void AddYaw(float value);
        void AddHeight(float value);
        void SetMinPitch(float value);
        void SetMaxPitch(float value);
		void SetFirstMouse(bool firstMouse) { _firstMouse = firstMouse; }
        const glm::mat4& GetViewMatrix() const;
		const glm::mat4& GetProjectionMatrix() const;
		const glm::mat4& GetViewProjectionMatrix() const;
        const glm::mat4& GetInverseViewMatrix() const;
        const glm::vec3& GetPosition() const;
        const glm::vec3& GetEulerRotation() const;
        const glm::vec3& GetForward() const;
        const glm::vec3& GetUp() const;
        const glm::vec3& GetRight() const;
        const glm::vec3 GetForwardXZ() const;
        const float GetPitch() const { return _rotation.x; }
        const float GetYaw() const { return _rotation.y; }
		const bool IsFirstMouse() const { return _firstMouse; }

    private:
        glm::vec3 _position = glm::vec3(0.0f);
        glm::vec3 _rotation = glm::vec3(0.0f);
        glm::mat4 _viewMatrix = glm::mat4(1.0f);
        glm::mat4 _projectionMatrix = glm::mat4(1.0f);
        glm::mat4 _inverseViewMatrix = glm::mat4(1.0f);
        glm::vec3 _forward = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 _forwardXZ = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 _right = glm::vec3(1.0f, 0.0f, 0.0f);
		float _aspectRatio = 1.0f;
        float _minPitch = -1.5f;
        float _maxPitch = 1.5f;
		bool _firstMouse = true;
	};
}