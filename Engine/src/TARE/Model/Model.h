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
		~Model();
		bool LoadFromFile(const std::string& filePath);
		void LoadModelAsync(const std::string& path, std::function<void(TAGE::MEM::Ref<Model>)> callback);
		void SetTransform(const glm::mat4& transform);

		void Draw(TAGE::MEM::Ref<Shader>& shader) const;
		void Draw(const std::string& shader) const;
		void DrawOutlined(const std::string& shader, const glm::mat4& viewProj);

		std::string GetFilePath() const { return _FilePath; }
		uint GetMeshCount() { return (uint)_meshes.size(); }
		Mesh& GetMesh(int slot) { return *_meshes[slot]; }
		BoundingBox GetBoundingBox() const;
		void AddMesh(TAGE::MEM::Scope<Mesh> mesh) { _meshes.push_back(std::move(mesh)); }

		bool LoadCPU(const std::string& path);
		void UploadToGPU();
	private:
		std::vector<TAGE::MEM::Scope<Mesh>> _meshes;
		std::string _Directory;
		std::string _FilePath;
		ModelType _Type = ModelType::MODEL;

		glm::mat4 _transform{ 1.0f };
		const aiScene* _scene = nullptr;

		void ProcessNode(aiNode* node, const aiScene* scene);
		void LoadTextureToMaterial(TextureType type, aiMaterial* material, TAGE::MEM::Ref<Material>& outMaterial);
		TAGE::MEM::Scope<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
	};
}