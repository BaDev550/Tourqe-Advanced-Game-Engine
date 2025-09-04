#include "tagepch.h"
#include "AssetManager.h"
#include "AssetImporter.h"
#include <yaml-cpp/yaml.h>

namespace TAGE {
    static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
        { ".png", AssetType::Texture },
        { ".jpg", AssetType::Texture },

        { ".fbx", AssetType::MeshSource },
        { ".obj", AssetType::MeshSource },
        { ".gltf", AssetType::MeshSource },

        { ".tmesh", AssetType::StaticMesh },
        { ".smesh", AssetType::SkeletalMesh },
		{ ".tmat",  AssetType::Material },

        { ".wav", AssetType::Audio },
        { ".mp3", AssetType::Audio },

        { ".scene", AssetType::Scene}
    };

    static AssetType GetAssetTypeFromExtension(const std::filesystem::path& e) {
        if (s_AssetExtensionMap.find(e) == s_AssetExtensionMap.end()) {
            return AssetType::None;
        }
        return s_AssetExtensionMap.at(e);
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

    AssetHandle AssetManagerEditor::TryToGetLoadedAssetFromPath(const std::filesystem::path& path) const
    {
        for (const auto& [asset, metadata] : _AssetRegistry) {
            if (metadata.FilePath == path)
                return asset;
        }
        return 0;
    }

    AssetHandle AssetManagerEditor::ImportAsset(const std::filesystem::path& path)
    {
        if (TryToGetLoadedAssetFromPath(path) != 0)
            return TryToGetLoadedAssetFromPath(path);

        AssetHandle handle;
        AssetMetadata metadata;
        metadata.FilePath = path;
        metadata.Type = GetAssetTypeFromExtension(path.extension());
        MEM::Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
        if (asset) {
            asset->_handle = handle;
            _LoadedAssets[handle] = asset;
            _AssetRegistry[handle] = metadata;
            SerializeAssetRegistry();
            return _LoadedAssets[handle]->_handle;
        }
        return 0;
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
            AssetHandle handle = node["Handle"].as<uint64>();
            auto& assetMetadata = _AssetRegistry[handle];
            assetMetadata.FilePath = node["FilePath"].as<std::string>();
            assetMetadata.Type = StringToAssetType(node["Type"].as<std::string>());
        }

        return true;
    }
}