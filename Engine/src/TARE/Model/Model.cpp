#include "tagepch.h"
#include "Model.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TAGE/AssetManager/AssetManager.h"

namespace TARE
{
	bool Model::LoadFromFile(const std::string& filePath)
	{
		Assimp::Importer importer;
		_scene = importer.ReadFile(filePath, 
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_GenNormals |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_OptimizeMeshes |
			aiProcess_ImproveCacheLocality |
			aiProcess_FindDegenerates |
			aiProcess_FindInvalidData |
			aiProcess_ValidateDataStructure |
			aiProcess_OptimizeGraph 
		);
		if (!_scene || !_scene->mRootNode)
		{
			LOG_ERROR("Failed to load model: {}. Error: {}", filePath, importer.GetErrorString());
			ASSERT(false, "Model load failed. See log for details.");
			return false;
		}
		if (_scene->HasSkeletons())
			_Type = ModelType::SKINNED_MODEL;

		_FilePath = filePath;
		_Directory = filePath.substr(0, filePath.find_last_of('/'));
		ProcessNode(_scene->mRootNode, _scene);
		return true;
	}

	void Model::Draw(TAGE::MEM::Ref<Shader>& shader) const
	{
		shader->Use();
		shader->SetUniform("u_Model", _transform);
		for (const auto& mesh : _meshes)
			mesh->Draw(shader);
	}

	void Model::Draw(const std::string& shader) const
	{
		TAGE::MEM::Ref<Shader> shaderRef = ShaderLibrary::Get(shader);
		shaderRef->Use();
		shaderRef->SetUniform("u_Model", _transform);
		for (const auto& mesh : _meshes)
			mesh->Draw(shaderRef);
	}

	BoundingBox Model::GetBoundingBox() const
	{
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);

		for (auto& mesh : _meshes) {
			for (auto& vertex : mesh->GetVertices()) {
				min = glm::min(min, vertex.pos);
				max = glm::max(max, vertex.pos);
			}
		}
		return { min, max };
	}

	void Model::SetTransform(const glm::mat4& transform)
	{
		_transform = transform;
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			_meshes.push_back(ProcessMesh(mesh, scene));
		}
		for (uint i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	aiTextureType ToAiTextureType(TextureType type) {
		switch (type) {
		case TextureType::DIFFUSE:            return aiTextureType_DIFFUSE;
		case TextureType::SPECULAR:           return aiTextureType_SPECULAR;
		case TextureType::NORMAL:             return aiTextureType_NORMALS;
		case TextureType::EMISSIVE:           return aiTextureType_EMISSIVE;
		case TextureType::HEIGHT:             return aiTextureType_HEIGHT;
		case TextureType::ROUGHNESS:          return aiTextureType_UNKNOWN;
		case TextureType::METALLIC:           return aiTextureType_METALNESS;
		case TextureType::AMBIENT_OCCLUSION:  return aiTextureType_LIGHTMAP;
		default:                              return aiTextureType_NONE;
		}
	}

	void Model::LoadTextureToMaterial(TextureType type, aiMaterial* material, TAGE::MEM::Ref<Material>& outMaterial)
	{
		aiTextureType aiType = ToAiTextureType(type);

		if (outMaterial->GetTexture(type))
			return;

		aiString path;
		if (material->GetTexture(aiType, 0, &path) == AI_SUCCESS && path.length > 0)
		{
			std::string texPath = path.C_Str();

			const aiTexture* aiTex = _scene->GetEmbeddedTexture(texPath.c_str());
			if (aiTex)
			{
				TAGE::MEM::Ref<Texture2D> texture = Texture2D::Create();
				if (aiTex->mHeight == 0)
					texture->LoadTextureFromMemory(reinterpret_cast<const unsigned char*>(aiTex->pcData), aiTex->mWidth);
				else
					texture->LoadTextureFromMemory(reinterpret_cast<const unsigned char*>(aiTex->pcData), aiTex->mWidth, aiTex->mHeight, 4);
				outMaterial->SetTexture(type, std::move(texture));
				return;
			}

			std::replace(texPath.begin(), texPath.end(), '\\', '/');
			std::filesystem::path modelDir = std::filesystem::path(_Directory).parent_path();
			std::filesystem::path textureFilename = std::filesystem::path(texPath).filename();
			std::filesystem::path fullPath = modelDir / textureFilename;

			std::filesystem::path fallback = std::filesystem::path(_Directory) / textureFilename;
			if (std::filesystem::exists(fallback)) {
				fullPath = fallback;
			}

			if (std::filesystem::exists(fullPath)) {
				TAGE::MEM::Ref<Texture2D> texture = Texture2D::Create();
				texture->LoadTexture(fullPath.string());
				outMaterial->SetTexture(type, std::move(texture));
			}
			else {
				LOG_WARN("Texture not found: {}", fullPath.string());
			}
		}
	}


	inline int8 QuantizeSigned(float value) { return static_cast<int8>(std::roundf(glm::clamp(value, -1.0f, 1.0f) * 127.0f)); }
	inline int16 QuantizePosition(float value) { return static_cast<int16>(value * 10000.0f); }
	inline uint16 QuantizeUV(float value) { return static_cast<uint16>(glm::clamp(value, 0.0f, 1.0f) * 65535.0f); }
	inline uint8 QuantizeWeight(float value) { return static_cast<uint8>(glm::clamp(value, 0.0f, 1.0f) * 255.0f); }

	TAGE::MEM::Scope<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<VertexData> vertices;
		std::vector<uint> indices;
		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			VertexData qv;
			
#ifdef TAGE_ENABLE_GLM_VERTEX_DATA
			qv.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			qv.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			if (mesh->HasNormals()) {
				qv.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}
			if (mesh->HasTangentsAndBitangents()) {
				qv.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				qv.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}
#else
			qv.pos[0] = QuantizePosition(mesh->mVertices[i].x * SCALE_FACTOR);
			qv.pos[1] = QuantizePosition(mesh->mVertices[i].y * SCALE_FACTOR);
			qv.pos[2] = QuantizePosition(mesh->mVertices[i].z * SCALE_FACTOR);

			if (mesh->HasNormals()) {
				qv.normal[0] = QuantizeSigned(mesh->mNormals[i].x);
				qv.normal[1] = QuantizeSigned(mesh->mNormals[i].y);
				qv.normal[2] = QuantizeSigned(mesh->mNormals[i].z);
			}

			if (mesh->mTextureCoords[0]) {
				qv.uv[0] = QuantizeUV(mesh->mTextureCoords[0][i].x);
				qv.uv[1] = QuantizeUV(mesh->mTextureCoords[0][i].y);
			}

			if (mesh->HasTangentsAndBitangents()) {
				qv.tangent[0] = QuantizeSigned(mesh->mTangents[i].x);
				qv.tangent[1] = QuantizeSigned(mesh->mTangents[i].y);
				qv.tangent[2] = QuantizeSigned(mesh->mTangents[i].z);

				qv.bitangent[0] = QuantizeSigned(mesh->mBitangents[i].x);
				qv.bitangent[1] = QuantizeSigned(mesh->mBitangents[i].y);
				qv.bitangent[2] = QuantizeSigned(mesh->mBitangents[i].z);
			}
#endif
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
		std::string meshName = mesh->mName.C_Str();
		if (meshName.empty()) meshName = "UnnamedMesh";

		TAGE::MEM::Ref<Material> material = TAGE::MEM::MakeRef<Material>(meshName.c_str());
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			LoadTextureToMaterial(TextureType::DIFFUSE, mat, material);
			LoadTextureToMaterial(TextureType::SPECULAR, mat, material);
			LoadTextureToMaterial(TextureType::NORMAL, mat, material);
			LoadTextureToMaterial(TextureType::HEIGHT, mat, material);
			LoadTextureToMaterial(TextureType::ROUGHNESS, mat, material);
			LoadTextureToMaterial(TextureType::METALLIC, mat, material);
			LoadTextureToMaterial(TextureType::AMBIENT_OCCLUSION, mat, material);
		}

		return TAGE::MEM::MakeScope<Mesh>(std::move(vertices), std::move(indices), std::move(material));
	}
}