#pragma once
#include "TAGE/Utilities/Memory.h"
#include "Asset.h"

namespace TAGE {
	using AssetMap = std::map<AssetHandle, MEM::Ref<Asset>>;
	class AssetManagerBase
	{
	public:
		virtual MEM::Ref<Asset> GetAsset(AssetHandle handle) const = 0;
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual AssetType GetAssetType(AssetHandle handle) const = 0;
	};
}