#pragma once
#include "TAGE/Utilities/UUID.h"

namespace TAGE {
	enum class AssetType {
		None = 0,
		Scene,
		MeshSource,
		StaticMesh,
		SkeletalMesh,
		Texture,
		Audio,
		Material
	};

	std::string_view AssetTypeToString(AssetType type);
	AssetType StringToAssetType(std::string_view type);

	using AssetHandle = UUID;
	class Asset
	{
	public:
		AssetHandle _handle;

		virtual AssetType GetType() const = 0;
	};
}