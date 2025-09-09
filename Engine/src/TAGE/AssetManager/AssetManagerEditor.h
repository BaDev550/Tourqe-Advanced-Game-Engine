#pragma once

#include "AssetManagerBase.h"
#include "AssetMetadata.h"
#include "AssetImporter.h"
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

		AssetHandle ImportAsset(const std::filesystem::path& path);
		MEM::Ref<Asset> LoadAsset(AssetHandle assetHandle);
		void SetMetadata(const AssetHandle& handle, AssetMetadata data);
		void SaveAsset(AssetHandle handle);
		void OnAssetRenamed(AssetHandle assetHandle, const std::filesystem::path& newFilePath);
		void RemoveAsset(AssetHandle handle);

		const AssetMetadata& GetMetadata(AssetHandle handle) const;
		const AssetMetadata& GetMetadata(const std::filesystem::path& filepath);
		const AssetRegistry& GetAssetRegistry() const { return _AssetRegistry; }
		virtual MEM::Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;
		std::vector<AssetHandle> GetHandlesWithType(AssetType type) const;
		AssetMap GetAssetsWithType(AssetType type) const;
		const std::filesystem::path& GetFilePath(AssetHandle handle) const;
		std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);
		std::filesystem::path GetFileSystemPath(const AssetMetadata& metadata);
		std::filesystem::path GetFileSystemPath(AssetHandle handle);
		std::string GetFileSystemPathString(const AssetMetadata& metadata);

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();

		template<typename T, typename... Args>
		MEM::Ref<T> CreateNewAsset(const std::string& filename, const std::string& directoryPath, Args&&... args)
		{
			static_assert(std::is_base_of<Asset, T>::value, "CreateNewAsset only works for types derived from Asset");

			AssetMetadata metadata;
			metadata.Handle = AssetHandle();
			if (directoryPath.empty() || directoryPath == ".")
				metadata.FilePath = filename;
			else
				metadata.FilePath = GetRelativePath(directoryPath + "/" + filename);
			metadata.IsLoaded = true;
			metadata.Type = T::GetStaticType();

			SetMetadata(metadata.Handle, metadata);
			SerializeAssetRegistry();

			MEM::Ref<T> asset = MEM::MakeRef<T>(std::forward<Args>(args)...);
			asset->_handle = metadata.Handle;
			_LoadedAssets[asset->_handle] = asset;
			AssetImporter::ImportAsset(asset->_handle, metadata);

			auto absolutePath = GetFileSystemPath(metadata);
			SetMetadata(metadata.Handle, metadata);
			return asset;
		}
	private:
		AssetRegistry _AssetRegistry;
		AssetMap _LoadedAssets;
	};
}