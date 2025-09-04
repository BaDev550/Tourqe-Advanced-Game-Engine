#pragma once
#include <string>
#include <filesystem>
#include "TAGE/AssetManager/Asset.h"

#include "glm/glm.hpp"
#include "TAGE/Common/TTypes.h"
#include "Mesh.h"
#include "SkinedMesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace TARE
{
	class Model : public TAGE::Asset
	{
	public:
		Model() = default;
		~Model();
		void Draw(const std::string& shader, const glm::mat4& transform) const;
	
		ModelType GetMeshType() const { return _Type; }
		std::vector<Mesh*> GetMeshes() const;
		std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() { return _boneInfoMap; }
		int& GetBoneCount() { return _BoneCounter; }
		void AddMesh(TAGE::MEM::Ref<Mesh>& mesh) { _meshes.push_back(std::move(mesh)); }

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::StaticMesh; };
		virtual TAGE::AssetType GetType() const { return GetStaticType(); };
	private:
		std::vector<TAGE::MEM::Ref<Mesh>> _meshes;
		std::unordered_map<std::string, BoneInfo> _boneInfoMap;
		int _BoneCounter = 0;
		ModelType _Type = ModelType::MODEL;

		void ProcessNode(aiNode* node, const aiScene* scene) {}
		void SetVertexBoneDataToDefault(SkinedVertexData& vertex) {}
		void SetVertexBoneData(SkinedVertexData& vertex, int boneID, float weight) {}
		void ExtractBoneWeightForVertex(std::vector<SkinedVertexData>& vertices, aiMesh* mesh, const aiScene* scene) {}
		void LoadTextureToMaterial(TextureType type, aiMaterial* material, TAGE::MEM::Ref<Material>& outMaterial) {}
		TAGE::MEM::Scope<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene) { return nullptr; }
		TAGE::MEM::Scope<Mesh> ProcessSkinnedMesh(aiMesh* mesh, const aiScene* scene) { return nullptr; }
		TAGE::MEM::Ref<Material> LoadMaterials(aiMesh* mesh, const aiScene* scene) { return nullptr; }
	};
}