#pragma once

#include "Mesh.h"
#include "TARE/Texture/Texture.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <vector>
#include <filesystem>

namespace TARE {
	class Model : public TAGE::Asset
	{
	public:
		Model() = default;
		Model(const std::filesystem::path& path);
		void AddMesh(TAGE::MEM::Ref<Mesh>& Mesh);

		std::vector<TAGE::MEM::Ref<Mesh>>& GetMeshes() { return _Meshes; }

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::MeshSource; };
		virtual TAGE::AssetType GetType() const { return GetStaticType(); };
	private:
		std::vector<TAGE::MEM::Ref<Mesh>> _Meshes;
	};
}
