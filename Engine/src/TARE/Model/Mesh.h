#pragma once

#include "VertexData.h"
#include "Material.h"

#include "TARE/Buffers/VertexArrayBuffer.h"

namespace TARE {
	class Mesh
	{
	public:
		Mesh(std::vector<VertexData> vertices, std::vector<uint> indices, TAGE::MEM::Ref<Material> material);
		Mesh() {}
		
		void SetMaterial(const TAGE::MEM::Ref<Material>& mat) { _Material = mat; }

		const TAGE::MEM::Ref<VertexArrayObject>& GetVertexArrayObject() { return _VAO; }
		const TAGE::MEM::Ref<Material>& GetMaterial() { return _Material; }
		const std::vector<VertexData> GetVertices() const { return _Vertices; }
		const std::vector<uint> GetIndices() const { return _Indices; }
	private:
		std::vector<VertexData> _Vertices;
		std::vector<uint> _Indices;
		TAGE::MEM::Ref<VertexArrayObject> _VAO;
		TAGE::MEM::Ref<Material> _Material;
	protected:
		void CreateMesh();
	};
}
