#pragma once
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Memory.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "TAGE/Renderer/Material/Material.h"

#include <vector>

namespace TAGE::Renderer {
	class Mesh {
	public:
		Mesh(
			std::vector<VertexData> vertices,
			std::vector<uint> indices,
			MEM::Ref<Material> material
		);
		Mesh() {}
		void Draw() const;
		std::vector<VertexData> GetVertices() { return _vertices; }
		std::vector<uint> GetIndices() { return _indices; }
		MEM::Ref<Material> GetMaterial() { return _material; }

		void SetVertices(const std::vector<VertexData>& data) { _vertices = data; }
		void SetIndices(const std::vector<uint>& data) { _indices = data; }
	protected:
		uint _VAO, _VBO, _EBO;
		std::vector<VertexData> _vertices;
		std::vector<uint> _indices;
		MEM::Ref<Material> _material;
		void SetupMesh();
	};
}