#pragma once
#include "TAGE/Utilities/Memory.h"
#include "TARE/Model/Model.h"
#include "TARE/Camera/Camera.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Project/Project.h"
#include "TARE/TARE3D.h"
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

	struct DirectionalLightComponent {
		glm::vec3 Color = glm::vec3(1.0f);
		float Intensity = 1.0f;
		bool CastShadows = true;
		
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
		DirectionalLightComponent() = default;
	};

	struct PointLightComponent {
		glm::vec3 Color = glm::vec3(1.0f);
		float Intensity = 1.0f;
		float Radius = 1.0f;
		float Falloff = 1.0f;
		bool CastShadows = true;
		
		PointLightComponent(const PointLightComponent&) = default;
		PointLightComponent() = default;
	};

	struct SpotLightComponent {
		glm::vec3 Color = glm::vec3(1.0f);
		float Intensity = 1.0f;
		float Range = 1.0f;
		float Falloff = 1.0f;
		bool CastShadows = true;

		SpotLightComponent(const SpotLightComponent&) = default;
		SpotLightComponent() = default;
	};

	struct CameraComponent {
		MEM::Ref<TARE::Camera> Handle;
		bool IsActive = true;

		CameraComponent() { Handle = MEM::MakeRef<TARE::Camera>(1280.0f, 720.0f); };
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const MEM::Ref<TARE::Camera>& camera) : Handle(camera) {}
	};
}