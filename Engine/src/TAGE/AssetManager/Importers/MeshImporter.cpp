#include "tagepch.h"
#include "MeshImporter.h"
#include "TAGE/Project/Project.h"

#include "TAGE/AssetManager/Serializers/AssetSerializer.h"

namespace TAGE {
	MEM::Ref<TARE::Model> MeshImporter::ImportSourceModel(AssetHandle handle, const AssetMetadata& metadata)
	{
		return SerializeModel(Project::GetAssetDirectory() / metadata.FilePath);
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

	MEM::Ref<TARE::Model> MeshImporter::SerializeModel(std::filesystem::path& filepath)
	{
		StaticMeshImporter sm_importer;

		MEM::Ref<TARE::Model> model = MEM::MakeRef<TARE::Model>();
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

		sm_importer.ProcessNode(filepath, model, scene->mRootNode, scene);
		StaticMeshSerializer::Serialize(model, filepath.replace_extension(".tmesh"));
		std::filesystem::path relativePath = std::filesystem::relative(filepath.replace_extension(".tmesh"), Project::GetAssetDirectory());
		Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);

		return model;
	}
}