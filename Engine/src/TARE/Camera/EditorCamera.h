#pragma once

#include "Camera.h"

namespace TARE {
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float win_Width, float win_Height, float fov = 45.0f, float nearClip = NEAR_CLIP, float farClip = FAR_CLIP) :
			Camera(win_Width, win_Height, fov, nearClip, farClip) {}

		virtual void OnUpdate(float dt) override;
		virtual void OnEvent(TAGE::Event& event) override;

	private:
		float _lastX = 0.0f;
		float _lastY = 0.0f;
	};
}