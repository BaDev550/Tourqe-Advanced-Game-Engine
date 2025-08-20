#pragma once
#include "TAGE/AssetManager/AssetMetadata.h"
#include "TAGE/World/Scene/Scene.h"

namespace TAGE {
	class SceneImporter
	{
	public:
		static MEM::Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
		static MEM::Ref<Scene> LoadScene(const std::filesystem::path& filepath);
	};
}