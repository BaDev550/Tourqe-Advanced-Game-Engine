#pragma once
#include "TAGE/Utilities/Memory.h"
#include "TARE/Model/Model.h"
#include "TARE/Camera/Camera.h"
#include "TAGE/Common/TTypes.h"

namespace TAGE {
	struct MeshComponent {
		MEM::Ref<TARE::Model> Handle;
		bool IsVisible = true;
		bool CastShadows = true;

		MeshComponent() = default;
		MeshComponent(const std::string& modelpath) {
			Handle = MEM::MakeRef<TARE::Model>();
			Handle->LoadFromFile(modelpath);
		}
	};

	struct LightComponent {
		Light Handle;

		LightComponent(Light light) : Handle(light) {}
		LightComponent() : Handle(Light()) {}
	};

	struct CameraComponent {
		MEM::Ref<TARE::Camera> Handle;

		CameraComponent() = default;
		CameraComponent(const MEM::Ref<TARE::Camera>& camera) : Handle(camera) {}
	};
}