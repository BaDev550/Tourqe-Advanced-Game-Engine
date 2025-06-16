#include "tagepch.h"
#include "Mesh.h"

namespace TAGE::Renderer
{
	Mesh::Mesh(std::vector<VertexData> vertices, std::vector<uint> indices, MEM::Ref<Material> material)
		: _vertices(std::move(vertices)), _indices(std::move(indices)), _material(std::move(material)) {
		SetupMesh();
	}

	void Mesh::Draw() const
	{
		if (_material)
			_material->Use();
		glBindVertexArray(_VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void Mesh::SetupMesh()
	{
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glGenBuffers(1, &_EBO);

		glBindVertexArray(_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VertexData), _vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint), _indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_SHORT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, pos));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_BYTE, GL_TRUE, sizeof(VertexData), (void*)offsetof(VertexData, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VertexData), (void*)offsetof(VertexData, uv));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_BYTE, GL_TRUE, sizeof(VertexData), (void*)offsetof(VertexData, tangent));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_BYTE, GL_TRUE, sizeof(VertexData), (void*)offsetof(VertexData, bitangent));


		glBindVertexArray(0);
	}
}