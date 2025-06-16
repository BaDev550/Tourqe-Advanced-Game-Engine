#include "tagepch.h"
//#include "AssetSerializer.h"
//
//namespace TAGE {
//    bool MaterialSerializer::Serialize(const Material& material, const std::string& outPath) {
//        std::ofstream out(outPath, std::ios::binary);
//        if (!out) return false;
//
//        MaterialFileHeader header;
//        uint32_t texCount = 0;
//        for (size_t i = 0; i < (size_t)TextureType::COUNT; i++)
//            if (material.GetTexture(static_cast<TextureType>(i)))
//                texCount++;
//
//        header.textureCount = texCount;
//        out.write(reinterpret_cast<const char*>(&header), sizeof(header));
//
//        for (size_t i = 0; i < (size_t)TextureType::COUNT; i++) {
//            TextureType slot = static_cast<TextureType>(i);
//            auto tex = material.GetTexture(slot);
//            if (!tex) continue;
//            std::string path = tex->GetPath();
//            if (path.empty()) continue;
//
//            uint32_t pathLen = (uint32_t)path.size();
//            out.write(reinterpret_cast<const char*>(&slot), sizeof(slot));
//            out.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
//            out.write(path.data(), pathLen);
//        }
//        return true;
//    }
//
//    bool MaterialSerializer::Deserialize(Material& material, const std::string& inPath) {
//        std::ifstream in(inPath, std::ios::binary);
//        if (!in) return false;
//
//        MaterialFileHeader header;
//        in.read(reinterpret_cast<char*>(&header), sizeof(header));
//        if (header.magic != MATERIAL_MAGIC) return false;  // 'MATL'
//
//        std::vector<std::pair<TextureType, std::string>> textures;
//        for (uint32_t i = 0; i < header.textureCount; i++) {
//            TextureType slot;
//            uint32_t pathLen;
//            in.read(reinterpret_cast<char*>(&slot), sizeof(slot));
//            in.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
//
//            std::string path(pathLen, '\0');
//            in.read(path.data(), pathLen);
//
//            textures.push_back({ slot, path });
//        }
//
//        material.SetTexturePaths(textures);
//        return true;
//    }
//}