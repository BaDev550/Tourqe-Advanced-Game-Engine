#pragma once
#include "TAGE/Utilities/Memory.h"
#include "TARE/Model/Model.h"
#include "TARE/Camera/Camera.h"
#include "TAGE/Common/TTypes.h"
#include "TARE/Skybox/Skybox.h"
#include "TARE/Animation/Animator.h"
#include "TARE/Animation/Animation.h"
#include "TARE/Common/Light.h"
#include "TAGE/Project/Project.h"
#include "TAGE/AssetManager/AssetManager.h"

namespace TAGE {
	struct MeshComponent {
		MEM::Ref<TARE::Model> Handle;
		bool IsVisible = true;
		bool CastShadows = true;
		bool IsSelected = false;
		bool IsSkinned = false;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& modelpath) {
			LoadMesh(modelpath);
		}
		MeshComponent(AssetHandle handle) {
			LoadMesh(handle);
		}
		~MeshComponent() {
			Handle.reset();
		}

		void LoadMesh(const std::string& meshPath) {
			std::filesystem::path path = std::filesystem::relative(meshPath, Project::GetAssetDirectory());
			AssetHandle asset = Project::GetActive()->GetEditorAssetManager()->ImportAsset(path);
			Handle = AssetManager::GetAsset<TARE::Model>(asset);
		}
		void LoadMesh(AssetHandle handle) {
			Handle = AssetManager::GetAsset<TARE::Model>(handle);
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

	struct AnimatorComponent {
		MEM::Ref<TARE::Animator> Handle;

		AnimatorComponent() = default;
		AnimatorComponent(const AnimatorComponent&) = default;
		AnimatorComponent(MEM::Ref<TARE::Animator> animator) : Handle(animator) {}
		AnimatorComponent(MEM::Ref<TARE::Animation> animation) {
			Handle = MEM::MakeRef<TARE::Animator>(animation);
		}

		void SetAnimation(const std::string& animationPath) {
			MEM::Ref<TARE::Animation> animation = MEM::MakeRef<TARE::Animation>(animationPath, _ModelHandle);
			if (Handle) {
				Handle->PlayAnimation(animation);
			}
			else {
				Handle = MEM::MakeRef<TARE::Animator>(animation);
			}
		}
	private:
		TARE::Model* _ModelHandle;

		friend class Scene;
	};
}