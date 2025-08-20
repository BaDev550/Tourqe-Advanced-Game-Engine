#include "tagepch.h"
#include "Asset.h"

namespace TAGE {
    std::string_view AssetTypeToString(AssetType type)
    {
        switch (type)
        {
        case TAGE::AssetType::None:   return "None";
        case TAGE::AssetType::Scene:  return "Scene";
        case TAGE::AssetType::MeshSource:   return "MeshSource";
		case TAGE::AssetType::StaticMesh: return "StaticMesh";
		case TAGE::AssetType::SkeletalMesh: return "SkeletalMesh";
		case TAGE::AssetType::Audio:  return "Audio";
        case TAGE::AssetType::Texture:return "Texture";
        case TAGE::AssetType::Material: return "Material";
        }
        return "<Invalid>";
    }

    AssetType StringToAssetType(std::string_view type)
    {
        if (type == "None")    return AssetType::None;
        if (type == "Scene")   return AssetType::Scene;
        if (type == "MeshSource")    return AssetType::MeshSource;
		if (type == "StaticMesh") return AssetType::StaticMesh;
		if (type == "SkeletalMesh") return AssetType::SkeletalMesh;
        if (type == "Audio") return AssetType::Audio;
        if (type == "Texture") return AssetType::Texture;
        if (type == "Material") return AssetType::Material;
        return AssetType::None;
    }
}