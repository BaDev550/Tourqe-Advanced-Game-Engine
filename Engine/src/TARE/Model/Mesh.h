#pragma once
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Memory.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "TARE/Material/Material.h"
#include "TARE/Buffers/Buffers.h"
#include "TARE/Buffers/VertexArrayBuffer.h"
#include "TARE/Shader/Shader.h"
#include <vector>

namespace TARE {
	class Mesh {
	public:
		Mesh(
			std::vector<VertexData> vertices,
			std::vector<uint> indices,
			TAGE::MEM::Ref<Material> material
		);
		Mesh() {}
		~Mesh();
		virtual void Draw(TAGE::MEM::Ref<Shader>& shader) const;

		std::vector<VertexData> GetVertices() { return _vertices; }
		std::vector<uint> GetIndices() { return _indices; }
		TAGE::MEM::Ref<Material> GetMaterial() { return _material; }

		void SetVertices(const std::vector<VertexData>& data) { _vertices = data; }
		void SetIndices(const std::vector<uint>& data) { _indices = data; }
	protected:
		std::vector<VertexData> _vertices;
		std::vector<uint> _indices;
		TAGE::MEM::Ref<VertexArrayObject> _VAO;
		TAGE::MEM::Ref<Material> _material;
		virtual void SetupMesh();
	};
}