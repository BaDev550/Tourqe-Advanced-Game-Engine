#include "tagepch.h"
#include "AssetManager.h"
#include "AssetImporter.h"
#include <yaml-cpp/yaml.h>
#include "AssetExtensions.h"

namespace TAGE {
    static AssetType GetAssetTypeFromExtension(const std::filesystem::path& e) {
        if (s_AssetExtensionMap.find(e) == s_AssetExtensionMap.end()) {
            return AssetType::None;
        }
        return s_AssetExtensionMap.at(e);
    }

    static AssetType GetAssetTypeFromPath(const std::filesystem::path& path)
    {
        return GetAssetTypeFromExtension(path.extension().string());
    }

    std::filesystem::path AssetManagerEditor::GetRelativePath(const std::filesystem::path& filepath)
    {
        std::filesystem::path relativePath = filepath.lexically_normal();
        std::string temp = filepath.string();
        if (temp.find(Project::GetAssetDirectory().string()) != std::string::npos)
        {
            relativePath = std::filesystem::relative(filepath, Project::GetAssetDirectory());
            if (relativePath.empty())
            {
                relativePath = filepath.lexically_normal();
            }
        }
        return relativePath;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v) {
        out << std::string(v.data(), v.size());
        return out;
    }

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

    const AssetMetadata& AssetManagerEditor::GetMetadata(const std::filesystem::path& filepath)
    {
        const auto relativePath = GetRelativePath(filepath);

        for (auto& [handle, metadata] : _AssetRegistry) {
            if (metadata.FilePath == relativePath)
                return metadata;
        }
        return {};
    }

    AssetMap AssetManagerEditor::GetAssetsWithType(AssetType type) const
    {
        AssetMap assetsOfType;
        for (const auto& [handle, asset] : _LoadedAssets) {
            if (GetAssetType(handle) == type) {
                assetsOfType[handle] = asset;
            }
        }
        return assetsOfType;
    }

    std::vector<AssetHandle> AssetManagerEditor::GetLoadedHandlesWithType(AssetType type) const
    {
        std::vector<AssetHandle> handles;
        for (const auto& [handle, asset] : _LoadedAssets) {
            if (GetAssetType(handle) == type) {
                handles.push_back(handle);
            }
        }
        return handles;
    }

    std::vector<AssetHandle> AssetManagerEditor::GetHandlesWithType(AssetType type) const
    {
        std::vector<AssetHandle> handles;
        for (const auto& [handle, asset] : _AssetRegistry) {
            if (GetAssetType(handle) == type) {
                handles.push_back(handle);
            }
        }
        return handles;
    }

    const std::filesystem::path& AssetManagerEditor::GetFilePath(AssetHandle handle) const
    {
		return GetMetadata(handle).FilePath;
    }


    MEM::Ref<Asset> AssetManagerEditor::GetAsset(AssetHandle handle)
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
            if (!asset) {
                LOG_ERROR("asset import failed");
            }
            _LoadedAssets[handle] = asset;
            _AssetRegistry[handle] = metadata;
        }
        return asset;
    }

    AssetType AssetManagerEditor::GetAssetType(AssetHandle handle) const
    {
        if (!IsAssetHandleValid(handle))
            return AssetType::None;

        return _AssetRegistry.at(handle).Type;
    }

    AssetHandle AssetManagerEditor::ImportAsset(const std::filesystem::path& path)
    {
        std::filesystem::path relativePath = GetRelativePath(path);

        if (auto& metadata = GetMetadata(path); metadata.IsValid())
            return metadata.Handle;

        AssetType type = GetAssetTypeFromPath(path);
        if (type == AssetType::None)
            return 0;

        AssetMetadata metadata;
        metadata.Handle = AssetHandle();
        metadata.FilePath = relativePath;
        metadata.Type = type;
        MEM::Ref<Asset> asset = AssetImporter::ImportAsset(metadata.Handle, metadata);
        if (asset) {
            asset->_handle = metadata.Handle;
            _LoadedAssets[metadata.Handle] = asset;
            SetMetadata(metadata.Handle, metadata);
            SerializeAssetRegistry();
            return metadata.Handle;
        }
        return 0;
    }

    void AssetManagerEditor::SetMetadata(const AssetHandle& handle, AssetMetadata data)
    {
        _AssetRegistry[handle] = data;
    }

    void AssetManagerEditor::OnAssetRenamed(AssetHandle assetHandle, const std::filesystem::path& newFilePath)
    {
        AssetMetadata data = GetMetadata(assetHandle);
        if (!data.IsValid())
            return;
        data.FilePath = newFilePath;
        SetMetadata(assetHandle, data);
        SerializeAssetRegistry();
    }

    void AssetManagerEditor::RemoveAsset(AssetHandle handle)
    {
        if (_LoadedAssets.find(handle) == _LoadedAssets.end())
            _LoadedAssets.erase(handle);

        {
            if (_AssetRegistry.find(handle) == _AssetRegistry.end())
                _AssetRegistry.erase(handle);
        }
        SerializeAssetRegistry();
    }

    void AssetManagerEditor::SaveAsset(AssetHandle handle)
    {
        if (!IsAssetHandleValid(handle))
            return;

        if (!IsAssetLoaded(handle))
            return;

        auto asset = _LoadedAssets.at(handle);
		AssetImporter::SaveAsset(asset, GetMetadata(handle));
    }

    void AssetManagerEditor::SerializeAssetRegistry()
    {
        auto path = Project::GetAssetRegistryPath();

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "AssetRegistry" << YAML::Value;

        out << YAML::BeginSeq;
        for (const auto [handle, metadata] : _AssetRegistry) {
            out << YAML::BeginMap;
            out << YAML::Key << "Handle" << YAML::Value << handle;
            std::string filepathStr = metadata.FilePath.generic_string();
            out << YAML::Key << "FilePath" << YAML::Value << filepathStr;
            out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;

        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
    }

    bool AssetManagerEditor::DeserializeAssetRegistry()
    {
        auto path = Project::GetAssetRegistryPath();
        YAML::Node data;
        try {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e) {
            LOG_ERROR("Failed to load asset registry: {}", e.what());
            return false;
        }

        auto registryNode = data["AssetRegistry"];
        if (!registryNode)
            return false;

        for (const auto node : registryNode) {
            AssetMetadata assetMetadata;
            assetMetadata.Handle = node["Handle"].as<uint64>();
            assetMetadata.FilePath = node["FilePath"].as<std::string>();
            assetMetadata.Type = StringToAssetType(node["Type"].as<std::string>());
            SetMetadata(assetMetadata.Handle, assetMetadata);
        }

        return true;
    }
}