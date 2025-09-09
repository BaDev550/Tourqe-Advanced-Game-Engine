#include "tagepch.h"
#include "MeshImporter.h"
#include "meshoptimizer.h"
#include "TAGE/Project/Project.h"

#include "TAGE/AssetManager/AssetSerializer.h"
#include "TAGE/AssetManager/Importers/TextureImporter.h"

namespace TAGE {
	void StaticMeshImporter::ProcessNode(const std::filesystem::path& filepath, MEM::Ref<TARE::Model>& model, aiNode* node, const aiScene* scene)
	{
		_SourcePath = filepath;

		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			model->AddMesh(std::move(ProcessMesh(model, mesh, scene)));
		}
		for (uint i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(filepath, model, node->mChildren[i], scene);
		}
	}

	aiTextureType ToAiTextureType(TextureType type) {
		switch (type) {
		case TextureType::DIFFUSE:            return aiTextureType_DIFFUSE;
		case TextureType::SPECULAR:           return aiTextureType_SPECULAR;
		case TextureType::NORMAL:             return aiTextureType_NORMALS;
		case TextureType::ROUGHNESS:          return aiTextureType_DIFFUSE_ROUGHNESS;
		case TextureType::METALLIC:           return aiTextureType_METALNESS;
		case TextureType::AMBIENT_OCCLUSION:  return aiTextureType_LIGHTMAP;
		default:                              return aiTextureType_NONE;
		}
	}

	void StaticMeshImporter::LoadTextureToMaterial(TextureType type, aiMaterial* material, MEM::Ref<TARE::Material>& outMaterial)
	{
		aiTextureType aiType = ToAiTextureType(type);

		if (outMaterial->GetTexture(type) != 0)
			return;

		aiString path;
		if (material->GetTexture(aiType, 0, &path) == AI_SUCCESS && path.length > 0)
		{
			std::filesystem::path sourceDir = _SourcePath.parent_path();
			std::filesystem::path textureSource = std::filesystem::absolute(sourceDir / path.C_Str());

			if (std::filesystem::exists(textureSource)) {
				const auto& assetMG = Project::GetActive()->GetEditorAssetManager();
				AssetHandle TextureHandle = assetMG->ImportAsset(textureSource);
				if (TextureHandle != 0) {
					outMaterial->SetTexture(type, TextureHandle);
				}
			}
		}
	}

	MEM::Ref<TARE::Material> StaticMeshImporter::LoadMaterials(aiMesh* mesh, const aiScene* scene)
	{
		MaterialSerializer mat_serializer;
		std::string materialName = "Material";
		MEM::Ref<TARE::Material> material = MEM::MakeRef<TARE::Material>();

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			materialName = mat->GetName().C_Str();
			LoadTextureToMaterial(TextureType::DIFFUSE, mat, material);
			LoadTextureToMaterial(TextureType::SPECULAR, mat, material);
			LoadTextureToMaterial(TextureType::NORMAL, mat, material);
			LoadTextureToMaterial(TextureType::ROUGHNESS, mat, material);
			LoadTextureToMaterial(TextureType::METALLIC, mat, material);
			LoadTextureToMaterial(TextureType::AMBIENT_OCCLUSION, mat, material);
			mat->Clear();
		}
		std::string cleanMaterialName = materialName;
		std::filesystem::path fullMaterialPath = (_SourcePath.parent_path() / cleanMaterialName);
		fullMaterialPath.replace_extension(".tmat");

		mat_serializer.Serialize(material, fullMaterialPath);
		AssetHandle materialHandle = Project::GetActive()->GetEditorAssetManager()->ImportAsset(fullMaterialPath);
		if (materialHandle != 0)
			material->_handle = materialHandle;

		return std::move(material);
	}

	MEM::Ref<TARE::Mesh> StaticMeshImporter::ProcessMesh(MEM::Ref<TARE::Model>& model, aiMesh* mesh, const aiScene* scene)
	{
		std::vector<VertexData> vertices;
		std::vector<uint> indices;
		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			VertexData qv;

			qv.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			qv.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			if (mesh->HasNormals()) {
				qv.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}
			if (mesh->HasTangentsAndBitangents()) {
				qv.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				qv.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}
			vertices.push_back(qv);
		}
		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			for (uint j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		std::vector<uint> remap(vertices.size());
		size_t unique_vertex_count = meshopt_generateVertexRemap(remap.data(), indices.data(), indices.size(), vertices.data(), vertices.size(), sizeof(VertexData));
		std::vector<uint> optimized_indices(indices.size());
		std::vector<VertexData> optimized_vertices(unique_vertex_count);

		meshopt_remapIndexBuffer(optimized_indices.data(), indices.data(), indices.size(), &remap[0]);
		meshopt_remapVertexBuffer(optimized_vertices.data(), vertices.data(), vertices.size(), sizeof(VertexData), &remap[0]);
		meshopt_optimizeVertexCache(optimized_indices.data(), optimized_indices.data(), optimized_indices.size(), unique_vertex_count);
		meshopt_optimizeOverdraw(optimized_indices.data(), optimized_indices.data(), optimized_indices.size(), &optimized_vertices[0].pos.x, unique_vertex_count, sizeof(VertexData), 1.05f);
		meshopt_optimizeVertexFetch(optimized_vertices.data(), optimized_indices.data(), optimized_indices.size(), optimized_vertices.data(), unique_vertex_count, sizeof(VertexData));

		vertices.clear();
		indices.clear();
		return TAGE::MEM::MakeRef<TARE::Mesh>(std::move(optimized_vertices), std::move(optimized_indices), std::move(LoadMaterials(mesh, scene)));
	}
}