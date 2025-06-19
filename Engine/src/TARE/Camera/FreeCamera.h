#pragma once
#include "Camera.h"
#include "TAGE/Utilities/Memory.h"

namespace TARE {
	class FreeCamera
	{
	public:
		FreeCamera();
		void Update(float dt);

		TAGE::MEM::Ref<Camera> GetCamera() const { return _Camera; }
	private:
		TAGE::MEM::Ref<Camera> _Camera;
		float _lastX = 0.0f;
		float _lastY = 0.0f;

		float _Speed = 3.0f;
	};
}