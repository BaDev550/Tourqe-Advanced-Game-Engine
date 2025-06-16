#pragma once
#include "TAGE/Renderer/Camera/Camera.h"
#include "TAGE/Utilities/Memory.h"

namespace TAGE {
	class FreeCamera
	{
	public:
		FreeCamera();
		void Update(float dt);

		MEM::Ref<Renderer::Camera> GetCamera() const { return _Camera; }
	private:
		MEM::Ref<Renderer::Camera> _Camera;
		float _lastX = 0.0f;
		float _lastY = 0.0f;

		float _Speed = 3.0f;
	};
}