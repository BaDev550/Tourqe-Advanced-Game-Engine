#pragma once
#include "TAGE/AssetManager/AssetMetadata.h"
#include "TARE/Model/Model.h"
#include "TARE/Texture/Texture.h"

namespace TAGE {
	class MeshImporter
	{
	public:
		static MEM::Ref<TARE::Model> ImportSourceModel(AssetHandle handle, const AssetMetadata& metadata);
		static MEM::Ref<TARE::Model> ImportStaticMesh(AssetHandle handle, const AssetMetadata& metadata);

	private:
		static MEM::Ref<TARE::Model> SerializeModel(std::filesystem::path& filepath);
		static MEM::Ref<TARE::Model> DeserializeModel(AssetHandle handle, std::filesystem::path& filepath);
	};

	class StaticMeshImporter {
	public:
		void ProcessNode(const std::filesystem::path& filepath, MEM::Ref<TARE::Model>& model, aiNode* node, const aiScene* scene);
	private:
		MEM::Ref<TARE::Mesh> ProcessMesh(MEM::Ref<TARE::Model>& model, aiMesh* mesh, const aiScene* scene);
		MEM::Ref<TARE::Material> LoadMaterials(aiMesh* mesh, const aiScene* scene);
		void LoadTextureToMaterial(TARE::TextureType type, aiMaterial* material, MEM::Ref<TARE::Material>& outMaterial);
		
		std::filesystem::path _SourcePath;
	};
}
