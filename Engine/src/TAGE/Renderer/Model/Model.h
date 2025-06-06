#pragma once
#include <string>
#include "glm/glm.hpp"
#include "TAGE/Common/TTypes.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "TAGE/Renderer/SkinnedModel/Animation.h"

namespace TAGE::Renderer
{
	class Model
	{
	public:
		Model() = default;
		bool LoadFromFile(const std::string& filePath);

		void Draw(const MEM::Ref<Shader>& shader) const;
		void Draw(const std::string& shader) const;

		glm::vec3 GetBoundingBoxMin() const;
		glm::vec3 GetBoundingBoxMax() const;
		void SetTransform(const glm::mat4& transform);

		const std::vector<Animation>& GetAnimations() const;
		const Animation* GetAnimationByName(const std::string& name) const;

	private:
		std::vector<MEM::Scope<Mesh>> _meshes;
		std::string _Directory;
		ModelType _Type = ModelType::MODEL;

		glm::vec3 _boundingBoxMin{ FLT_MAX, FLT_MAX, FLT_MAX };
		glm::vec3 _boundingBoxMax{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
		glm::mat4 _transform{ 1.0f };
		const aiScene* _scene = nullptr;
		std::vector<Animation> _animations;

		void ProcessNode(aiNode* node, const aiScene* scene);
		void LoadTextureToMaterial(TextureType type, aiMaterial* material, MEM::Scope<Material>& outMaterial);
		MEM::Scope<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
		MEM::Scope<Mesh> ProcessSkinnedMesh(aiMesh* mesh, const aiScene* scene);
		void LoadAnimations(const aiScene* scene);
	};
}