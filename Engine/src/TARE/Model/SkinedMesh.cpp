#include "tagepch.h"
#include "SkinedMesh.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE {
	void SkinedMesh::Draw(TAGE::MEM::Ref<Shader>& shader) const
	{
		if (_material)
			_material->Use(shader);
		RenderCommand::DrawIndexed(_VAO);
	}

	void SkinedMesh::SetupMesh()
	{
		TAGE::MEM::Ref<VertexBufferObject> _VBO;
		_VBO = VertexBufferObject::Create(_vertices.data(), _vertices.size() * sizeof(SkinedVertexData));
#ifdef TAGE_ENABLE_GLM_VERTEX_DATA
		BufferLayout layout = {
			{ ShaderDataType::VEC3,  "aPos" },
			{ ShaderDataType::VEC3,  "aNormal",    true },
			{ ShaderDataType::VEC2,  "aTexCoord",  true },
			{ ShaderDataType::VEC3,  "aTangent",   true },
			{ ShaderDataType::VEC3,  "aBitangent", true },
			{ ShaderDataType::INT,   "aBoneIDs",     true },
			{ ShaderDataType::FLOAT, "aBoneWeights", true }
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