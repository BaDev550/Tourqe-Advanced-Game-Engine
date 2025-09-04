#include "tagepch.h"
#include "Model.h"
#include "TAGE/Application/Application.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TAGE/Project/Project.h"
#include "meshoptimizer.h"
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TMath.h"
#include "TAGE/AssetManager/AssetManager.h"
#include "TAGE/AssetManager/Importers/TextureImporter.h"
#include "TAGE/AssetManager/Asset.h"

namespace TARE
{
	Model::~Model() {
		_meshes.clear();
	}

	void Model::Draw(const std::string& shader, const glm::mat4& transform) const
	{
		if (_meshes.empty()) return;

		TAGE::MEM::Ref<Shader> shaderRef = ShaderLibrary::Get(shader);
		shaderRef->Use();
		shaderRef->SetUniform("u_Model", transform);
		for (const auto& mesh : _meshes) {
			mesh->Draw(shaderRef);
		}
	}

#if 0
	void Model::SetVertexBoneDataToDefault(SkinedVertexData& vertex)
	{
		for (uint i = 0; i < MAX_BONE_INFLUENCES; ++i)
		{
			vertex.BoneIDs[i] = -1;
			vertex.BoneWeights[i] = 0.0f;
		}
	}

	void Model::SetVertexBoneData(SkinedVertexData& vertex, int boneID, float weight)
	{
		for(int i = 0; i < MAX_BONE_INFLUENCES; ++i)
		{
			if (vertex.BoneIDs[i] < 0.0f) {
				vertex.BoneWeights[i] = weight;
				vertex.BoneIDs[i] = boneID;
				break;
			}
		}
	}

	void Model::ExtractBoneWeightForVertex(std::vector<SkinedVertexData>& vertices, aiMesh* mesh, const aiScene* scene)
	{
		auto& boneInfoMap = _boneInfoMap;
		int& boneCount = _BoneCounter;
		for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
				BoneInfo newBoneInfo;
				newBoneInfo.Id = boneCount;
				newBoneInfo.Offset = TAGE::Math::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else {
				boneID = boneInfoMap[boneName].Id;
			}
			ASSERT_NOMSG(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (int i = 0; i < numWeights; ++i) {
				int vertexID = weights[i].mVertexId;
				float weight = weights[i].mWeight;
				SetVertexBoneData(vertices[vertexID], boneID, weight);
			}
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

	void Model::LoadTextureToMaterial(TextureType type, aiMaterial* material, TAGE::MEM::Ref<Material>& outMaterial)
	{
		aiTextureType aiType = ToAiTextureType(type);

		if (outMaterial->GetTexture(type) != 0)
			return;

		aiString path;
		if (material->GetTexture(aiType, 0, &path) == AI_SUCCESS && path.length > 0)
		{
			std::filesystem::path modelDir = _FilePath.parent_path();
			std::filesystem::path assetDir = std::filesystem::absolute(TAGE::Project::GetAssetDirectory());
			std::filesystem::path texturePath = std::filesystem::absolute(modelDir / path.C_Str());
			std::filesystem::path relativePath = std::filesystem::relative(texturePath, assetDir);

			if (std::filesystem::exists(TAGE::Project::GetAssetDirectory() / relativePath)) {
				const auto& assetMG = TAGE::Project::GetActive()->GetEditorAssetManager();
				TAGE::AssetHandle TextureHandle = assetMG->ImportAsset(relativePath);
				if (TextureHandle != 0)
					outMaterial->SetTexture(type, TextureHandle);
			}
			else {
				LOG_WARN("Texture not found: {}", texturePath.string());
			}
		}
	}

	TAGE::MEM::Scope<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
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
		return TAGE::MEM::MakeScope<Mesh>(std::move(optimized_vertices), std::move(optimized_indices), std::move(LoadMaterials(mesh, scene)));
	}

	TAGE::MEM::Scope<Mesh> Model::ProcessSkinnedMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<SkinedVertexData> vertices;
		std::vector<uint> indices;
		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			SkinedVertexData qv;
			SetVertexBoneDataToDefault(qv);
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
		size_t unique_vertex_count = meshopt_generateVertexRemap(remap.data(), indices.data(), indices.size(), vertices.data(), vertices.size(), sizeof(SkinedVertexData));
		std::vector<uint> optimized_indices(indices.size());
		std::vector<SkinedVertexData> optimized_vertices(unique_vertex_count);

		meshopt_remapIndexBuffer(optimized_indices.data(), indices.data(), indices.size(), &remap[0]);
		meshopt_remapVertexBuffer(optimized_vertices.data(), vertices.data(), vertices.size(), sizeof(SkinedVertexData), &remap[0]);
		meshopt_optimizeVertexCache(optimized_indices.data(), optimized_indices.data(), optimized_indices.size(), unique_vertex_count);
		meshopt_optimizeOverdraw(optimized_indices.data(), optimized_indices.data(), optimized_indices.size(), &optimized_vertices[0].pos.x, unique_vertex_count, sizeof(SkinedVertexData), 1.05f);
		meshopt_optimizeVertexFetch(optimized_vertices.data(), optimized_indices.data(), optimized_indices.size(), optimized_vertices.data(), unique_vertex_count, sizeof(SkinedVertexData));
		ExtractBoneWeightForVertex(optimized_vertices, mesh, scene);

		return TAGE::MEM::MakeScope<SkinedMesh>(std::move(optimized_vertices), std::move(optimized_indices), std::move(LoadMaterials(mesh, scene)));
	}

	TAGE::MEM::Ref<Material> Model::LoadMaterials(aiMesh* mesh, const aiScene* scene)
	{
		std::string meshName = mesh->mName.C_Str();
		if (meshName.empty()) meshName = "UnnamedMesh";
		TAGE::MEM::Ref<Material> material = TAGE::MEM::MakeRef<Material>(meshName.c_str());

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			LoadTextureToMaterial(TextureType::DIFFUSE, mat, material);
			LoadTextureToMaterial(TextureType::SPECULAR, mat, material);
			LoadTextureToMaterial(TextureType::NORMAL, mat, material);
			LoadTextureToMaterial(TextureType::ROUGHNESS, mat, material);
			LoadTextureToMaterial(TextureType::METALLIC, mat, material);
			LoadTextureToMaterial(TextureType::AMBIENT_OCCLUSION, mat, material);
			mat->Clear();
		}
		return std::move(material);
	}
#endif

	std::vector<Mesh*> Model::GetMeshes() const
	{
		std::vector<Mesh*> meshes;
		meshes.reserve(_meshes.size());
		for (const auto& mesh : _meshes)
			meshes.push_back(mesh.get());
		return meshes;
	}
}