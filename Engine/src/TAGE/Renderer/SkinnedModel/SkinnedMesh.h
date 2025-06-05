#pragma once
#include "TAGE/Renderer/Model/Mesh.h"

namespace TAGE::Renderer {
    class SkinnedMesh : public Mesh {
    public:
        SkinnedMesh(std::vector<SkinedVertexData> vertices, std::vector<uint> indices, MEM::Scope<Material> material) 
        {
            _skinnedVertices = std::move(vertices);
            _indices = std::move(indices);
            _material = std::move(material);

			LoadSkinnedMesh();
        }
    private:
        std::vector<SkinedVertexData> _skinnedVertices;

        void LoadSkinnedMesh() {
			glGenVertexArrays(1, &_VAO);
			glGenBuffers(1, &_VBO);
			glGenBuffers(1, &_EBO);

			glBindVertexArray(_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferData(GL_ARRAY_BUFFER, _skinnedVertices.size() * sizeof(SkinedVertexData), _skinnedVertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint), _indices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinedVertexData), (void*)offsetof(SkinedVertexData, Position));

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinedVertexData), (void*)offsetof(SkinedVertexData, Normal));

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinedVertexData), (void*)offsetof(SkinedVertexData, TexCoord));

			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkinedVertexData), (void*)offsetof(SkinedVertexData, Tangent));

			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkinedVertexData), (void*)offsetof(SkinedVertexData, Bitangent));

			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT, sizeof(SkinedVertexData), (void*)offsetof(SkinedVertexData, BoneIDs));

			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(SkinedVertexData), (void*)offsetof(SkinedVertexData, BoneWeights));

			glBindVertexArray(0);
        }
    };
}