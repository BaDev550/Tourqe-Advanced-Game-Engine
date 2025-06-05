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
			MEM::Scope<Material> material
		);
		Mesh() {}
		void Draw() const;
	protected:
		uint _VAO, _VBO, _EBO;
		std::vector<VertexData> _vertices;
		std::vector<uint> _indices;
		MEM::Scope<Material> _material;
		void SetupMesh();
	};
}