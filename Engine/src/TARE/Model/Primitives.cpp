#include "tagepch.h"
#include "Primitives.h"
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include <vector>

namespace Primitives {

	void CreateWireSphere(uint& vao, uint& vbo) {
		std::vector<glm::vec3> vertices;
		const int segments = 32;

		for (int i = 0; i <= segments; i++) {
			float theta = 2.0f * PI * i / segments;
			vertices.emplace_back(cos(theta), sin(theta), 0.0f); // XY
		}

		for (int i = 0; i <= segments; i++) {
			float theta = 2.0f * PI * i / segments;
			vertices.emplace_back(0.0f, cos(theta), sin(theta)); // YZ
		}

		for (int i = 0; i <= segments; i++) {
			float theta = 2.0f * PI * i / segments;
			vertices.emplace_back(cos(theta), 0.0f, sin(theta)); // XZ
		}

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

		glBindVertexArray(0);
	}

	void CreateWireCone(uint& vao, uint& vbo) {
		std::vector<glm::vec3> vertices;
		const int segments = 32;

		const glm::vec3 tip(0.0f, 0.0f, 1.0f);
		const float radius = 1.0f;

		std::vector<glm::vec3> baseCircle;
		for (int i = 0; i <= segments; i++) {
			float theta = 2.0f * PI * i / segments;
			float x = cos(theta) * radius;
			float y = sin(theta) * radius;
			baseCircle.emplace_back(x, y, 0.0f);
		}

		for (int i = 0; i < segments; i++) {
			vertices.emplace_back(tip);
			vertices.emplace_back(baseCircle[i]);
		}

		for (int i = 0; i < segments; i++) {
			vertices.emplace_back(baseCircle[i]);
			vertices.emplace_back(baseCircle[i + 1]);
		}

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

		glBindVertexArray(0);
	}

	void CreatePointVAO(uint& vao, uint& vbo) {
		const glm::vec3 point(0.0f, 0.0f, 0.0f);

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &point, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

		glBindVertexArray(0);
	}

	void CreateArrowVAO(uint& vao, uint& vbo) {
		const float arrowVertices[] = {
			0.f, 0.f, 0.f,
			1.f, 0.f, 0.f
		};

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(arrowVertices), arrowVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glBindVertexArray(0);
	}

} 
