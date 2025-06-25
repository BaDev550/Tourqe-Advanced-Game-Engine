#pragma once
#include "System_Base.h"
#include "TARE/Camera/EditorCamera.h"
#include "TAGE/Common/TTypes.h"
#include "TARE/TARE.h"
#include "TARE/Skybox/Skybox.h"

namespace TAGE {
	class System_Renderer : public System {
	public:
		System_Renderer(TARE::TARE* renderer);
		virtual void Update(float deltaTime) override;
		virtual void UpdateEditor(float deltaTime) override;
		void SetEditorCamera(const MEM::Ref<TARE::EditorCamera>& camera) { _EditorCamera = camera; }
	private:
		void Render(const MEM::Ref<TARE::Camera>& cam);
		void RenderObjects();
		void RenderShadowObject();
		void GetLights(std::vector<Light>& lights, MEM::Ref<TARE::Skybox>& skybox);
	private:
		MEM::Ref<TARE::EditorCamera> _EditorCamera;
		TARE::TARE* _Renderer;
	};
}