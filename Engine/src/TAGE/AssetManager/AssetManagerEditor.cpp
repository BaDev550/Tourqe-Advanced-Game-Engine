#include "tagepch.h"
#include "AssetManager.h"
#include "AssetImporter.h"

namespace TAGE {
    bool AssetManagerEditor::IsAssetHandleValid(AssetHandle handle) const
    {
        if (handle == 0)
            return false;

        return _AssetRegistry.find(handle) != _AssetRegistry.end();
    }

    bool AssetManagerEditor::IsAssetLoaded(AssetHandle handle) const
    {
        return _LoadedAssets.find(handle) != _LoadedAssets.end();
    }

    const AssetMetadata& AssetManagerEditor::GetMetadata(AssetHandle handle) const
    {
        static AssetMetadata s_NullMetadata;
        auto it = _AssetRegistry.find(handle);
        if (it == _AssetRegistry.end())
            return s_NullMetadata;

        return it->second;
    }

    MEM::Ref<Asset> AssetManagerEditor::GetAsset(AssetHandle handle) const
    {
        if (!IsAssetHandleValid(handle))
            return nullptr;

        MEM::Ref<Asset> asset;
        if (IsAssetLoaded(handle)) {
            asset = _LoadedAssets.at(handle);
            return asset;
        }
        else {
            const AssetMetadata& metadata = GetMetadata(handle);
            asset = AssetImporter::ImportAsset(handle, metadata);
            if (!asset) {}
        }
        return asset;
    }
}