#pragma once
#include "System_Base.h"
#include "TARE/Camera/EditorCamera.h"
#include "TAGE/Common/TTypes.h"

namespace TAGE {
	class System_Renderer : public System {
	public:
		System_Renderer();
		virtual void Update(float deltaTime) override;
		virtual void UpdateEditor(float deltaTime) override;
		void SetEditorCamera(const MEM::Ref<TARE::EditorCamera>& camera) { _EditorCamera = camera; }
	private:
		void Render(const MEM::Ref<TARE::Camera>& cam, float dt, bool runtime = false);
		void RenderObjects(bool runtime);
	private:
		MEM::Ref<TARE::EditorCamera> _EditorCamera;
		float _DeltaTime = 0.0f;
	};
}