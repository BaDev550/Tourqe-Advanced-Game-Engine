#include "tagepch.h"
#include "Model.h"
#include "TARE/Shader/ShaderLibrary.h"

namespace TARE {
	Model::Model(const std::filesystem::path& path)
	{
	}

	void Model::AddMesh(TAGE::MEM::Ref<Mesh>& Mesh) { _Meshes.push_back(Mesh); }
}