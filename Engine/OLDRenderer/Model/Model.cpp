#include "tagepch.h"
#include "Model.h"
#include "TAGE/Renderer/SkinnedModel/SkinnedMesh.h"
#include "TAGE/Renderer/Shader/ShaderLibrary.h"
#include "TAGE/AssetManager/AssetManager.h"

namespace TAGE::Renderer
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
			aiProcess_OptimizeGraph |
			aiProcess_EmbedTextures |
			aiProcess_GlobalScale 
		);
		if (!_scene || !_scene->mRootNode)
		{
			LOG_ERROR("Failed to load model: {}. Error: {}", filePath, importer.GetErrorString());
			ASSERT(false, "Model load failed. See log for details.");
			return false;
		}
		if (_scene->HasSkeletons())
			_Type = ModelType::SKINNED_MODEL;

		_Directory = filePath.substr(0, filePath.find_last_of('/'));
		ProcessNode(_scene->mRootNode, _scene);
		return true;
	}

	void Model::Draw(const MEM::Ref<Shader>& shader) const
	{
		shader->Use();
		for (const auto& mesh : _meshes)
			mesh->Draw();
	}

	void Model::Draw(const std::string& shader) const
	{
		MEM::Ref<Shader> shaderRef = ShaderLibrary::Get(shader);
		shaderRef->Use();
		shaderRef->SetUniform("u_Model", _transform);
		for (const auto& mesh : _meshes)
			mesh->Draw();
	}

	glm::vec3 Model::GetBoundingBoxMin() const { return glm::vec3(); }
	glm::vec3 Model::GetBoundingBoxMax() const { return glm::vec3(); }

	void Model::SetTransform(const glm::mat4& transform)
	{
		_transform = transform;
		_transform = glm::scale(transform, glm::vec3(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR));
	}

	const std::vector<Animation>& Model::GetAnimations() const { return _animations; }
	const Animation* Model::GetAnimationByName(const std::string& name) const {
		for (const auto& anim : _animations) {
			if (anim.Name == name)
				return &anim;
		}
		return nullptr;
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			if (_Type == ModelType::SKINNED_MODEL)
				_meshes.push_back(ProcessSkinnedMesh(mesh, scene));
			else
				_meshes.push_back(ProcessMesh(mesh, scene));
		}
		for (uint i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene);
		}
		if (_Type == ModelType::SKINNED_MODEL)
			LoadAnimations(scene);
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

	void Model::LoadTextureToMaterial(TextureType type, aiMaterial* material, MEM::Ref<Material>& outMaterial)
	{
		aiTextureType aiType = ToAiTextureType(type);

		if (outMaterial->GetTexture(type))
			return;

		aiString path;
		if (material->GetTexture(aiType, 0, &path) == AI_SUCCESS)
		{
			std::string texPath = path.C_Str();
			
			if (_scene->GetEmbeddedTexture(texPath.c_str()))
			{
				int texIndex = atoi(texPath.c_str() + 1);
				const aiTexture* aiTex = _scene->mTextures[texIndex];
				if (aiTex)
				{
					MEM::Ref<Texture2D> texture;
					std::string embeddedName = "_embedded_" + std::to_string(texIndex);

					if (aiTex->mHeight == 0)
						texture = AssetManager::ImportEmbeddedTexture(embeddedName, reinterpret_cast<const unsigned char*>(aiTex->pcData), aiTex->mWidth);
					else
						texture = AssetManager::ImportEmbeddedTexture(embeddedName, reinterpret_cast<const unsigned char*>(aiTex->pcData), aiTex->mWidth, aiTex->mHeight, 4);

					outMaterial->SetTexture(type, std::move(texture));
					return;
				}
			}
			else {
				std::string fullPath = _Directory + "/" + texPath;
				MEM::Ref<Texture2D> texture = AssetManager::ImportTexture(fullPath);

				outMaterial->SetTexture(type, std::move(texture));
				return;
			}
		}
	}

	MEM::Scope<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<VertexData> vertices;
		std::vector<uint> indices;
		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			VertexData qv;

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

		MEM::Ref<Material> material = MEM::MakeRef<Material>(mesh->mName.C_Str(), "MainShader");
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

		return MEM::MakeScope<Mesh>(std::move(vertices), std::move(indices), std::move(material));
	}

	MEM::Scope<Mesh> Model::ProcessSkinnedMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<SkinedVertexData> vertices;
		std::vector<uint> indices;
		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			SkinedVertexData vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->HasNormals())
				vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			if (mesh->mTextureCoords[0])
			{
				vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertex.TexCoord = glm::vec2(0.0f, 0.0f);
			}

			if (mesh->HasTangentsAndBitangents()) {
				vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}
			
			vertices.push_back(vertex);
		}
		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			for (uint j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		
		MEM::Ref<Material> material = AssetManager::ImportMaterial(mesh->mName.C_Str());
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

		return MEM::MakeScope<SkinnedMesh>(std::move(vertices), std::move(indices), std::move(material));
	}
	void Model::LoadAnimations(const aiScene* scene)
	{
		_animations.clear();
		for (uint i = 0; i < _scene->mNumAnimations; ++i)
		{
			aiAnimation* aiAnim = _scene->mAnimations[i];
			Animation animation;
			animation.Name = aiAnim->mName.C_Str();
			animation.Duration = static_cast<float>(aiAnim->mDuration);
			animation.TicksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond != 0.0 ? aiAnim->mTicksPerSecond : 25.0f); // fallback

			for (uint j = 0; j < aiAnim->mNumChannels; ++j)
			{
				aiNodeAnim* channel = aiAnim->mChannels[j];
				BoneAnimation boneAnim;
				boneAnim.BoneName = channel->mNodeName.C_Str();

				for (uint k = 0; k < channel->mNumPositionKeys; ++k)
				{
					Keyframe pos;
					pos.Position = glm::vec3(channel->mPositionKeys[k].mValue.x,
						channel->mPositionKeys[k].mValue.y,
						channel->mPositionKeys[k].mValue.z);
					pos.Time = static_cast<float>(channel->mPositionKeys[k].mTime);
					boneAnim.PositionKeys.push_back(pos);
				}

				for (uint k = 0; k < channel->mNumRotationKeys; ++k)
				{
					Keyframe rot;
					rot.Rotation = glm::quat(channel->mRotationKeys[k].mValue.w,
						channel->mRotationKeys[k].mValue.x,
						channel->mRotationKeys[k].mValue.y,
						channel->mRotationKeys[k].mValue.z);
					rot.Time = static_cast<float>(channel->mRotationKeys[k].mTime);
					boneAnim.RotationKeys.push_back(rot);
				}

				for (uint k = 0; k < channel->mNumScalingKeys; ++k)
				{
					Keyframe scale;
					scale.Scale = glm::vec3(channel->mScalingKeys[k].mValue.x,
						channel->mScalingKeys[k].mValue.y,
						channel->mScalingKeys[k].mValue.z);
					scale.Time = static_cast<float>(channel->mScalingKeys[k].mTime);
					boneAnim.ScaleKeys.push_back(scale);
				}

				animation.BoneAnimations[boneAnim.BoneName] = boneAnim;
			}

			_animations.push_back(std::move(animation));
		}
	}
}