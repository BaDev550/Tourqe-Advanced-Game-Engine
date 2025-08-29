#pragma once
#include "Mesh.h"
#include "TAGE/Common/TTypes.h"

namespace TARE {
	class SkinedMesh : public Mesh
	{
	public:
		SkinedMesh(
			std::vector<SkinedVertexData> vertices,
			std::vector<uint> indices,
			TAGE::MEM::Ref<Material> material
		);
		SkinedMesh() {}

		virtual void SetVertices(const std::vector<SkinedVertexData>& data) { _skinnedVertices = data; }
		virtual void SetupMesh() override;
		std::vector<SkinedVertexData> GetSkinnedVertices() { return _skinnedVertices; }
	protected:
		std::vector<SkinedVertexData> _skinnedVertices;
	};
}