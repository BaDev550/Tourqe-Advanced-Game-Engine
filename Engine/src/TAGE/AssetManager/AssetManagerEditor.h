#pragma once

#include "AssetManagerBase.h"
#include "AssetMetadata.h"
#include <map>

namespace TAGE {
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	class AssetManagerEditor : public AssetManagerBase
	{
	public:
		~AssetManagerEditor() {
			_AssetRegistry.clear();
			_LoadedAssets.clear();
		}

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual MEM::Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;
		AssetHandle TryToGetLoadedAssetFromPath(const std::filesystem::path& path) const;

		AssetHandle ImportAsset(const std::filesystem::path& path);
		AssetHandle ImportAsset(const std::filesystem::path& where, const std::filesystem::path& to);
		void SaveAsset(AssetHandle handle);

		const AssetMetadata& GetMetadata(AssetHandle handle) const;
		const AssetRegistry& GetAssetRegistry() const { return _AssetRegistry; }
		std::vector<AssetHandle> GetHandlesWithType(AssetType type) const;
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;
		AssetMap GetAssetsWithType(AssetType type) const;

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();
	private:
		AssetRegistry _AssetRegistry;
		AssetMap _LoadedAssets;
	};
}