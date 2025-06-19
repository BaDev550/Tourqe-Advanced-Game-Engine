#pragma once
#include <string>
#include "glm/glm.hpp"
#include "TAGE/Common/TTypes.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace TARE
{
	class Model
	{
	public:
		Model() = default;
		bool LoadFromFile(const std::string& filePath);
		void SetTransform(const glm::mat4& transform);

		void Draw(const TAGE::MEM::Ref<Shader>& shader) const;
		void Draw(const std::string& shader) const;

		uint GetMeshCount() { return (uint)_meshes.size(); }
		Mesh& GetMesh(int slot) { return *_meshes[slot]; }
		void AddMesh(TAGE::MEM::Scope<Mesh> mesh) { _meshes.push_back(std::move(mesh)); }
	private:
		std::vector<TAGE::MEM::Scope<Mesh>> _meshes;
		std::string _Directory;
		ModelType _Type = ModelType::MODEL;

		glm::mat4 _transform{ 1.0f };
		const aiScene* _scene = nullptr;

		void ProcessNode(aiNode* node, const aiScene* scene);
		void LoadTextureToMaterial(TextureType type, aiMaterial* material, TAGE::MEM::Ref<Material>& outMaterial);
		TAGE::MEM::Scope<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
	};
}