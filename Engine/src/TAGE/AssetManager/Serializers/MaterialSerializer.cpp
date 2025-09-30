#include "tagepch.h"
#include "TARE/Model/Material.h"
#include "TAGE/Project/Project.h"
#include "AssetSerializer.h"

#include <yaml-cpp/yaml.h>

namespace YAML {
    template<>
    struct convert<TAGE::UUID> {
        static Node encode(const TAGE::UUID& uuid) {
            Node node;
            node.push_back((uint64)uuid);
            return node;
        }
        static bool decode(const Node& node, TAGE::UUID& uuid) {
            uuid = node[0].as<uint64>();
            return true;
        }
    };
}
namespace TAGE {
    bool TAGE::MaterialSerializer::Serialize(const MEM::Ref<Asset>& asset, const std::filesystem::path& path)
    {
        auto material = std::dynamic_pointer_cast<TARE::Material>(asset);
        if (!material)
        {
            LOG_ERROR("Asset: {} is not a Material", (uint64)asset->_handle);
            return false;
        }
        const auto& textures = material->GetTextures();

        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        if (!out.is_open()) {
            LOG_ERROR("Failed to open file for writing: {}", path.string());
            return false;
        }

        const uint magic = 0x4D41544C; // "MATL"
        const uint version = 1;
        out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        out.write(reinterpret_cast<const char*>(&version), sizeof(version));
        out.write(reinterpret_cast<const char*>(&textures), sizeof(TARE::TextureMaps));
		return true;
    }

    MEM::Ref<Asset> TAGE::MaterialSerializer::Deserialize(const std::filesystem::path& path)
    {
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open()) {
            LOG_ERROR("Failed to open Material: {}", path.string());
            return nullptr;
        }
        
        uint magic;
        uint version;
        in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        in.read(reinterpret_cast<char*>(&version), sizeof(version));

        if (magic != 0x4D41544C) { // "MATL"
            LOG_ERROR("Invalid Material file format: {}", path.string());
            return nullptr;
        }

        TARE::TextureMaps maps{};
        in.read(reinterpret_cast<char*>(&maps), sizeof(maps));

        auto material = TARE::Material::Create();
        material->SetTextures(maps);

        return material;
    }
}