#include "tagepch.h"
#include "SkinedMesh.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE {
	SkinedMesh::SkinedMesh(std::vector<SkinedVertexData> vertices, std::vector<uint> indices, TAGE::MEM::Ref<Material> material)
	{
		_vertices.clear();
		_skinnedVertices = std::move(vertices);
		_indices = std::move(indices);
		_material = std::move(material);
		SetupMesh();
	}

	void SkinedMesh::SetupMesh()
	{
		TAGE::MEM::Ref<VertexBufferObject> _VBO;
		_VBO = VertexBufferObject::Create(_skinnedVertices.data(), _skinnedVertices.size() * sizeof(SkinedVertexData));
		BufferLayout layout = {
			{ ShaderDataType::VEC3,  "aPos" },
			{ ShaderDataType::VEC3,  "aNormal",      true },
			{ ShaderDataType::VEC2,  "aTexCoord",    true },
			{ ShaderDataType::VEC3,  "aTangent",     true },
			{ ShaderDataType::VEC3,  "aBitangent",   true },
			{ ShaderDataType::IVEC4, "aBoneIDs"     },
			{ ShaderDataType::VEC4,  "aBoneWeights" }
		};

		_VAO = VertexArrayObject::Create();
		_VBO->SetLayout(layout);
		_VAO->AddVertexBuffer(_VBO);

		TAGE::MEM::Ref<ElementBufferObject> _EBO;
		_EBO = ElementBufferObject::Create(_indices.data(), _indices.size());

		_VAO->SetIndexBuffer(_EBO);
	}
}