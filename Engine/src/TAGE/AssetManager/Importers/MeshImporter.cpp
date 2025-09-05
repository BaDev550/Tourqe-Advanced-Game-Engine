#include "tagepch.h"
#include "MeshImporter.h"
#include "TAGE/Project/Project.h"

#include "TAGE/AssetManager/AssetSerializer.h"

namespace TAGE {
	MEM::Ref<TARE::Model> MeshImporter::ImportSourceModel(AssetHandle handle, const AssetMetadata& metadata)
	{
		return SerializeModel(Project::GetAssetDirectory() / metadata.InputPath, Project::GetAssetDirectory() / metadata.FilePath);
	}

	MEM::Ref<TARE::Model> MeshImporter::ImportStaticMesh(AssetHandle handle, const AssetMetadata& metadata)
	{
		return DeserializeModel(handle, Project::GetAssetDirectory() / metadata.FilePath);
	}

	MEM::Ref<TARE::Model> MeshImporter::DeserializeModel(AssetHandle handle, std::filesystem::path& filepath)
	{
		MEM::Ref<TARE::Model> model = std::dynamic_pointer_cast<TARE::Model>(StaticMeshSerializer::Deserialize(filepath));
		model->_handle = handle;

		return model;
	}

	MEM::Ref<TARE::Model> MeshImporter::SerializeModel(std::filesystem::path& filepath, std::filesystem::path& to)
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
			sm_importer.ProcessNode(to, model, scene->mRootNode, scene);
			StaticMeshSerializer::Serialize(model, to.replace_extension(".tmesh"));
		}

		return model;
	}
}