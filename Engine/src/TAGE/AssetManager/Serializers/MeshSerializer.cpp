#include "tagepch.h"
#include "TARE/Model/Model.h"
#include "TAGE/Project/Project.h"

#include "TAGE/AssetManager/AssetManager.h"
#include "TAGE/AssetManager/AssetSerializer.h"

#include <yaml-cpp/yaml.h>

namespace TAGE {
    bool TAGE::StaticMeshSerializer::Serialize(const MEM::Ref<Asset>& asset, const std::filesystem::path& path)
    {
		auto staticMesh = std::dynamic_pointer_cast<TARE::Model>(asset);
        if (!staticMesh)
        {
            LOG_ERROR("Asset: {} is not a StaticMesh", (uint64)asset->_handle);
            return false;
		}

        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        if (!out.is_open()) {
            LOG_ERROR("Failed to open file for writing: {}", path.string());
            return false;
        }
        const uint magic = 0x4D4F444C; // "MODL"
        const uint version = 1;
        out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        out.write(reinterpret_cast<const char*>(&version), sizeof(version));

        const auto& meshes = staticMesh->GetMeshes();
        uint meshCount = (uint)meshes.size();
        out.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

        for (const auto& mesh : meshes)
        {
            const auto& vertices = mesh->GetVertices();
            uint vertexCount = (uint)vertices.size();
            out.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
            out.write(reinterpret_cast<const char*>(vertices.data()), sizeof(VertexData) * vertexCount);

            const auto& indices = mesh->GetIndices();
            uint indexCount = (uint)indices.size();
            out.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
            out.write(reinterpret_cast<const char*>(indices.data()), sizeof(uint) * indexCount);

            AssetHandle matHandle = mesh->GetMaterial() ? mesh->GetMaterial()->_handle : 0;
            out.write(reinterpret_cast<const char*>(&matHandle), sizeof(AssetHandle));
        }

        return true;
    }

    MEM::Ref<Asset> TAGE::StaticMeshSerializer::Deserialize(const std::filesystem::path& path)
    {
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open()) {
            LOG_ERROR("Failed to open StaticMesh: {}", path.string());
            return nullptr;
        }
        uint magic;
        uint version;
        in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        in.read(reinterpret_cast<char*>(&version), sizeof(version));

        if (magic != 0x4D4F444C) { // "MODL"
            LOG_ERROR("Invalid StaticMesh file format: {}", path.string());
            return nullptr;
        }

        auto staticMesh = MEM::MakeRef<TARE::Model>();

        uint meshCount;
        in.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
        staticMesh->GetMeshes().reserve(meshCount);

        for (uint i = 0; i < meshCount; ++i)
        {
            uint vertexCount;
            in.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
            std::vector<VertexData> vertices(vertexCount);
            in.read(reinterpret_cast<char*>(vertices.data()), sizeof(VertexData) * vertexCount);

            uint indexCount;
            in.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
            std::vector<uint> indices(indexCount);
            in.read(reinterpret_cast<char*>(indices.data()), sizeof(uint) * indexCount);

            AssetHandle matHandle;
            in.read(reinterpret_cast<char*>(&matHandle), sizeof(AssetHandle));

            MEM::Ref<TARE::Material> material = nullptr;
            if (matHandle != 0)
                material = AssetManager::GetAsset<TARE::Material>(matHandle);

            auto mesh = MEM::MakeRef<TARE::Mesh>(vertices, indices, material);
            staticMesh->AddMesh(mesh);
        }

        return staticMesh;
    }
}