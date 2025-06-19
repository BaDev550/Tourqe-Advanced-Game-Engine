#pragma once
#include "System_Base.h"
#include "TARE/TARE.h"

namespace TAGE {
	class System_Renderer : public System {
	public:
		System_Renderer(TARE::TARE* renderer);
		virtual void Update(SystemUpdateType updateType, float deltaTime) override;
		void RenderObjects(const std::string& shaderName = "MainShader");
	private:
		TARE::TARE* _Renderer;
	};
}