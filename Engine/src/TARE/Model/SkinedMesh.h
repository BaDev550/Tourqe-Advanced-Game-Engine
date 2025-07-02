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

		virtual void Draw(TAGE::MEM::Ref<Shader>& shader) const override;
		virtual void SetVertices(const std::vector<SkinedVertexData>& data) { _skinnerVertices = data; }
		std::vector<SkinedVertexData> GetSkinnedVertices() { return _skinnerVertices; }
	protected:
		std::vector<SkinedVertexData> _skinnerVertices;
		virtual void SetupMesh();
	};
}