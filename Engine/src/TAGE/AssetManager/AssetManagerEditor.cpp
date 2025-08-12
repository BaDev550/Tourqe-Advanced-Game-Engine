#include "tagepch.h"
#include "AssetManager.h"
#include "AssetImporter.h"
#include <yaml-cpp/yaml.h>

namespace TAGE {
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
            if (!asset) {
                LOG_ERROR("asset import failed");
            }
        }
        return asset;
    }

    void AssetManagerEditor::SerializeAssetRegistry()
    {
        auto path = Project::GetAssetRegistryPath();

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "AssetRegistry" << YAML::Value;
        for (const auto [handle, metadata] : _AssetRegistry) {
            out << YAML::BeginMap;
            out << YAML::Key << "Handle" << YAML::Value << handle;
            out << YAML::Key << "FilePath" << YAML::Value << metadata.FilePath.string();
            out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
            out << YAML::EndMap;
        }
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