#pragma once
#include <string>
#include <filesystem>

#include "glm/glm.hpp"
#include "TAGE/Common/TTypes.h"
#include "Mesh.h"
#include "SkinedMesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace TARE
{
	class Model
	{
	public:
		Model() = default;
		~Model();
		bool LoadFromFile(const std::string& filePath);
		void LoadModelAsync(const std::string& path, std::function<void(TAGE::MEM::Ref<Model>)> callback);
		void Draw(const std::string& shader, const glm::mat4& transform) const;

	public:
		std::string GetFilePath() const { return _FilePath.string(); }

		ModelType GetType() const { return _Type; }
		std::vector<Mesh*> GetMeshes() const;
		std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() { return _boneInfoMap; }
		int& GetBoneCount() { return _BoneCounter; }
		void AddMesh(TAGE::MEM::Scope<Mesh> mesh) { _meshes.push_back(std::move(mesh)); }

		bool LoadCPU(const std::string& path);
		void UploadToGPU();
	private:
		std::vector<TAGE::MEM::Scope<Mesh>> _meshes;
		std::unordered_map<std::string, BoneInfo> _boneInfoMap;
		int _BoneCounter = 0;
		std::filesystem::path _FilePath;
		ModelType _Type = ModelType::MODEL;

		const aiScene* _scene = nullptr;

		void ProcessNode(aiNode* node, const aiScene* scene);
		void SetVertexBoneDataToDefault(SkinedVertexData& vertex);
		void SetVertexBoneData(SkinedVertexData& vertex, int boneID, float weight);
		void ExtractBoneWeightForVertex(std::vector<SkinedVertexData>& vertices, aiMesh* mesh, const aiScene* scene);
		void LoadTextureToMaterial(TextureType type, aiMaterial* material, TAGE::MEM::Ref<Material>& outMaterial);
		TAGE::MEM::Scope<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
		TAGE::MEM::Scope<Mesh> ProcessSkinnedMesh(aiMesh* mesh, const aiScene* scene);
		TAGE::MEM::Ref<Material> LoadMaterials(aiMesh* mesh, const aiScene* scene);
	};
}