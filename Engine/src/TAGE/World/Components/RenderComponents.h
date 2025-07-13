#pragma once
#include "TAGE/Utilities/Memory.h"
#include "TARE/Model/Model.h"
#include "TARE/Camera/Camera.h"
#include "TAGE/Common/TTypes.h"
#include "TARE/Skybox/Skybox.h"
#include "TARE/Common/Light.h"

namespace TAGE {
	struct MeshComponent {
		MEM::Ref<TARE::Model> Handle;
		bool IsVisible = true;
		bool CastShadows = true;
		bool IsSelected = false;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& modelpath) {
			Handle = MEM::MakeRef<TARE::Model>();

			Handle->LoadModelAsync(modelpath, [&](TAGE::MEM::Ref<TARE::Model> model) {
				if (model) { Handle = std::move(model); }
				else { LOG_ERROR("Failed to load async model"); }
				});
		}
		~MeshComponent() {
			Handle.reset();
		}

		void LoadMesh(const std::string& meshPath) {
			Handle.reset(new TARE::Model());

			Handle->LoadModelAsync(meshPath, [&](TAGE::MEM::Ref<TARE::Model> model) {
				if (model) { Handle = std::move(model); }
				else { LOG_ERROR("Failed to load async model"); }
				});
		}
	};

	struct LightComponent {
		Light Handle;

		LightComponent(Light light) : Handle(light) {}
		LightComponent(const LightComponent&) = default;
		LightComponent() : Handle(Light()) {}
	};

	struct CameraComponent {
		MEM::Ref<TARE::Camera> Handle;
		bool IsActive = true;

		CameraComponent() { Handle = MEM::MakeRef<TARE::Camera>(1280.0f, 720.0f); };
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const MEM::Ref<TARE::Camera>& camera) : Handle(camera) {}
	};

	struct SkyboxComponent {
		MEM::Ref<TARE::Skybox> Handle;

		SkyboxComponent() = default;
		SkyboxComponent(const SkyboxComponent&) = default;
		SkyboxComponent(const std::string& cubemapPath) {
			Handle = MEM::MakeRef<TARE::Skybox>(cubemapPath.c_str());
		}
	};
}