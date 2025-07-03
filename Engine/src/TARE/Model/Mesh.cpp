#include "tagepch.h"
#include "Mesh.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE
{
	Mesh::Mesh(std::vector<VertexData> vertices, std::vector<uint> indices, TAGE::MEM::Ref<Material> material)
		: _vertices(std::move(vertices)), _indices(std::move(indices)), _material(std::move(material)) {
		SetupMesh();
	}

	void Mesh::Draw(TAGE::MEM::Ref<Shader>& shader) const
	{
		if (_material)
			_material->Use(shader);
		RenderCommand::DrawIndexed(_VAO);
	}

	Mesh::~Mesh() {
		if (_VAO) {
			_VAO->Destroy();
			_VAO.reset();
		}
		_vertices.clear();
		_indices.clear();
	}

	void Mesh::SetupMesh()
	{
		TAGE::MEM::Ref<VertexBufferObject> _VBO;
		_VBO = VertexBufferObject::Create(_vertices.data(), _vertices.size() * sizeof(VertexData));
#ifdef TAGE_ENABLE_GLM_VERTEX_DATA
		BufferLayout layout = {
			{ ShaderDataType::VEC3,  "aPos" },
			{ ShaderDataType::VEC3,  "aNormal",      true },
			{ ShaderDataType::VEC2,  "aTexCoord",    true },
			{ ShaderDataType::VEC3,  "aTangent",     true },
			{ ShaderDataType::VEC3,  "aBitangent",   true }
		};
#else
		BufferLayout layout = {
			{ ShaderDataType::SHORT3,      "aPos" },
			{ ShaderDataType::BYTE3_NORM,  "aNormal",    true },
			{ ShaderDataType::USHORT2,     "aTexCoord",  true },
			{ ShaderDataType::BYTE3_NORM,  "aTangent",   true },
			{ ShaderDataType::BYTE3_NORM,  "aBitangent", true }
		};
#endif
		_VAO = VertexArrayObject::Create();
		_VBO->SetLayout(layout);
		_VAO->AddVertexBuffer(_VBO);

		TAGE::MEM::Ref<ElementBufferObject> _EBO;
		_EBO = ElementBufferObject::Create(_indices.data(), _indices.size());

		_VAO->SetIndexBuffer(_EBO);
	}
}