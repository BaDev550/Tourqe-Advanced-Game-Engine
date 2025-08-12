#include "tagepch.h"
#include "Asset.h"

namespace TAGE {
    std::string_view AssetTypeToString(AssetType type)
    {
        switch (type)
        {
        case TAGE::AssetType::None:   return "None";
        case TAGE::AssetType::Scene:  return "Scene";
        case TAGE::AssetType::Mesh:   return "Mesh";
        case TAGE::AssetType::Texture:return "Texture";
        case TAGE::AssetType::Material: return "Material";
        }
        return "<Invalid>";
    }

    AssetType StringToAssetType(std::string_view type)
    {
        if (type == "None")    return AssetType::None;
        if (type == "Scene")   return AssetType::Scene;
        if (type == "Mesh")    return AssetType::Mesh;
        if (type == "Texture") return AssetType::Texture;
        if (type == "Material") return AssetType::Material;
        return AssetType::None;
    }
}