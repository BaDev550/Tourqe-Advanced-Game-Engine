#include "tagepch.h"
#include "Mesh.h"

namespace TARE {
	Mesh::Mesh(std::vector<VertexData> vertices, std::vector<uint> indices, TAGE::MEM::Ref<Material> material)
		: _Vertices(vertices), _Indices(indices), _Material(material) {
		CreateMesh();
	}

	void Mesh::CreateMesh() {
		TAGE::MEM::Ref<VertexBufferObject> VBO;
		VBO = VertexBufferObject::Create(_Vertices.data(), _Vertices.size() * sizeof(VertexData));
		BufferLayout layout = {
			{ ShaderDataType::VEC3, "aPos" },
			{ ShaderDataType::VEC3, "aNormal",   true},
			{ ShaderDataType::VEC2, "aTexCoords",true},
			{ ShaderDataType::VEC3, "aTangent",  true},
			{ ShaderDataType::VEC3, "aBitangent",true}
		};
		VBO->SetLayout(layout);

		_VAO = VertexArrayObject::Create();
		_VAO->AddVertexBuffer(VBO);

		TAGE::MEM::Ref<ElementBufferObject> EBO;
		EBO = ElementBufferObject::Create(_Indices.data(), _Indices.size());

		_VAO->SetIndexBuffer(EBO);
	}
}