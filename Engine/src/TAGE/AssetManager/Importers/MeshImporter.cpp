#include "tagepch.h"
#include "MeshImporter.h"
#include "TAGE/Project/Project.h"

#include "TAGE/AssetManager/AssetSerializer.h"

namespace TAGE {
	MEM::Ref<TARE::Model> MeshImporter::ImportSourceModel(AssetHandle handle, const AssetMetadata& metadata)
	{
		return SerializeModel(metadata.FilePath);
	}

	MEM::Ref<TARE::Model> MeshImporter::ImportStaticMesh(AssetHandle handle, const AssetMetadata& metadata)
	{
		return DeserializeModel(handle, metadata.FilePath);
	}

	MEM::Ref<TARE::Model> MeshImporter::DeserializeModel(AssetHandle handle, const std::filesystem::path& filepath)
	{
		MEM::Ref<TARE::Model> model = std::dynamic_pointer_cast<TARE::Model>(StaticMeshSerializer::Deserialize(filepath));
		model->_handle = handle;

		return model;
	}

	MEM::Ref<TARE::Model> MeshImporter::SerializeModel(const std::filesystem::path& filepath)
	{
		StaticMeshImporter sm_importer;

		MEM::Ref<TARE::Model> model = MEM::MakeRef<TARE::Model>();
		ModelType type = ModelType::MODEL;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.string(),
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_ImproveCacheLocality |
			aiProcess_FindDegenerates |
			aiProcess_FindInvalidData |
			aiProcess_ValidateDataStructure
		);
		if (!scene || !scene->mRootNode) {
			LOG_ERROR("Failed to load model: {}. Error: {}", filepath.string(), importer.GetErrorString());
			return false;
		}
		bool hasBones = false;
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			if (scene->mMeshes[i]->HasBones()) {
				hasBones = true;
				break;
			}
		}
		type = hasBones ? ModelType::SKINNED_MODEL : ModelType::MODEL;

		if (type == ModelType::MODEL) {
			sm_importer.ProcessNode(filepath, model, scene->mRootNode, scene);
			std::filesystem::path serializedPath = filepath;
			StaticMeshSerializer::Serialize(model, serializedPath.replace_extension(".tmesh"));
			Project::GetActive()->GetEditorAssetManager()->ImportAsset(serializedPath.replace_extension(".tmesh"));
		}

		return model;
	}
}