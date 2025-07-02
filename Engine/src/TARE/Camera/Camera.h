#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "TAGE/Events/Event.h"

namespace TARE
{
	class Camera
	{
	public:
        Camera(float win_Width = 1280, float win_Height = 720, float fov = 45.0f, float nearClip = NEAR_CLIP, float farClip = FAR_CLIP) {
            _aspectRatio = win_Width / win_Height;
            _fov = fov;
            _nearClip = nearClip;
            _farClip = farClip;

            _projectionMatrix = glm::perspective(glm::radians(_fov), _aspectRatio, _nearClip, _farClip);
        }


        virtual void OnUpdate(float dt) {};
		virtual void OnEvent(TAGE::Event& event) {}

        void CalculateCameraMatrixes();
        void SetPosition(glm::vec3 position);
        void SetEulerRotation(glm::vec3 rotation);
        void SetQuaternionRotation(const glm::quat& quat);
        void Orbit(float pitchOffset, float yawOffset);
        void AddPitch(float value);
        void AddYaw(float value);
        void AddHeight(float value);
        void SetMinPitch(float value);
        void SetMaxPitch(float value);
        void OnResize(float win_Width, float win_Height);
		void SetFirstMouse(bool firstMouse) { _firstMouse = firstMouse; }
		void SetAspectRatio(float aspectRatio) { _aspectRatio = aspectRatio; }
		void SetFOV(float fov) { _fov = fov; }
		void SetNearClip(float nearClip) { _nearClip = nearClip; }
		void SetFarClip(float farClip) { _farClip = farClip; }
        void LookAt(glm::vec3 target);
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
        
		float GetAspectRatio() const { return _aspectRatio; }
		float GetFOV() const { return _fov; }
		float GetNearClip() const { return _nearClip; }
		float GetFarClip() const { return _farClip; }
    private:
        glm::vec3 _position = glm::vec3(0.0f);
        glm::vec3 _rotation = glm::vec3(0.0f);
        glm::mat4 _viewMatrix = glm::mat4(1.0f);
        glm::mat4 _viewProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 _projectionMatrix = glm::mat4(1.0f);
        glm::mat4 _inverseViewMatrix = glm::mat4(1.0f);
        glm::vec3 _forward = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 _forwardXZ = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 _right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::quat _orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        float _minPitch = -1.5f;
        float _maxPitch = 1.5f;
		bool _firstMouse = true;

		float _aspectRatio = 1.0f;
        float _fov{ 45.0f };
        float _nearClip{ 0.1f };
        float _farClip{ 1000.0f };
	};
}