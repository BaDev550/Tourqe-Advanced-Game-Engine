#pragma once
#include "TAGE/AssetManager/Asset.h"
#include "TAGE/Utilities/Memory.h"
#include <filesystem>

namespace TAGE {
	class StaticMeshSerializer {
	public:
		static bool Serialize(const MEM::Ref<Asset>& asset, const std::filesystem::path& path);
		static MEM::Ref<Asset> Deserialize(const std::filesystem::path& path);
	};

	class MaterialSerializer {
	public:
		static bool Serialize(const MEM::Ref<Asset>& asset, const std::filesystem::path& path);
		static MEM::Ref<Asset> Deserialize(const std::filesystem::path& path);
	};
}