#pragma once
#include "TAGE/Common/TEnums.h"
#include "TAGE/Utilities/Memory.h"

namespace TAGE {
	class Scene;
	class System {
	public:
		virtual ~System() = default;
		virtual void Update(float deltaTime) = 0;
		virtual void UpdateEditor(float deltaTime) = 0;
		virtual void StartRuntime() {}
		virtual void StopRuntime() {}

		void SetActiveScene(Scene* scene) { _Scene = scene; }
	protected:
		Scene* _Scene;
	};
}