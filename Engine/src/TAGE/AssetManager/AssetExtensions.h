#pragma once
#include <map>
#include "AssetManager.h"

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
}