#include "tagepch.h"
#include "DebugRenderer.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE::Debug {

	void DebugRenderer::Init()
	{
		_VBO = VertexBufferObject::Create(nullptr, 10000);
		BufferLayout layout = {
			{ ShaderDataType::VEC3, "aPos" },
			{ ShaderDataType::VEC3, "aColor" }
		};

		_VBO->SetLayout(layout);
		_VAO = VertexArrayObject::Create();
		_VAO->AddVertexBuffer(_VBO);

		_debugShader = ShaderLibrary::Add("DebugSingleColorShader", "../Engine/shaders/Debugger/single_color_vertex", "../Engine/shaders/Debugger/single_color_fragment");
		_initialized = true;
	}

	void DebugRenderer::BeginFrame()
	{
		_vertices.clear();
	}

	void DebugRenderer::DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float duration) {
		_commands.push_back({ DebugPrimitiveType::LINE, duration, color, from, to });
	}

	void DebugRenderer::DrawSphere(const glm::vec3& center, float radius, const glm::vec3& color, float duration) {
		_commands.push_back({ DebugPrimitiveType::SPHERE, duration, color, center, {}, radius });
	}
	
	void DebugRenderer::DrawCapsule(const glm::vec3& center, float height, float radius, const glm::vec3& color, float duration) {
		_commands.push_back({ DebugPrimitiveType::CAPSULE, duration, color, center, {}, radius, height });
	}

	void DebugRenderer::EndFrame(float deltaTime)
	{
		if (!_initialized) Init();

		GenerateGeometry();

		if (!_vertices.empty())
			_VBO->SetData(_vertices.data(), _vertices.size() * sizeof(DebugVertexData));
		else
			_VBO->SetData(nullptr, 0);

		_commands.erase(std::remove_if(_commands.begin(), _commands.end(),
			[deltaTime](const DebugDrawCommand& cmd) {
				if (cmd.TimeRemaining < 0.0f) {
					return false;
				}
				return (cmd.TimeRemaining - deltaTime) <= 0.0f;
			}), _commands.end());

		for (auto& cmd : _commands)
		{
			if (cmd.TimeRemaining > 0.0f)
			{
				cmd.TimeRemaining -= deltaTime;
			}
		}
	}

	void DebugRenderer::Render(const glm::mat4& viewProj)
	{
		uint vertexCount = _vertices.size();
		if (vertexCount == 0)
			return;

		_debugShader->Use();
		_debugShader->SetUniform("u_ViewProj", viewProj);

		RenderCommand::Disable(DEPTH_TEST);
		RenderCommand::Disable(CULL_TEST);
		RenderCommand::DrawLine(_VAO, vertexCount);
		RenderCommand::Enable(CULL_TEST);
		RenderCommand::Enable(DEPTH_TEST);
	}

	void DebugRenderer::GenerateGeometry()
	{
		for (const auto& cmd : _commands) {
			switch (cmd.Type) {
			case DebugPrimitiveType::LINE:
				_vertices.push_back({ cmd.P1, cmd.Color });
				_vertices.push_back({ cmd.P2, cmd.Color });
				break;
			case DebugPrimitiveType::SPHERE:
				GenerateSphere(cmd.P1, cmd.Radius, cmd.Color);
				break;
			case DebugPrimitiveType::CAPSULE:
				GenerateCapsule(cmd.P1, cmd.Height, cmd.Radius, cmd.Color);
				break;
			}
		}
	}

	void DebugRenderer::GenerateSphere(const glm::vec3& center, float radius, const glm::vec3& color)
	{
		const int segments = 12;
		for (int i = 0; i < segments; ++i) {
			float theta1 = (float)i / segments * glm::two_pi<float>();
			float theta2 = (float)(i + 1) / segments * glm::two_pi<float>();

			// XY circle
			glm::vec3 p1 = center + glm::vec3(cos(theta1), sin(theta1), 0) * radius;
			glm::vec3 p2 = center + glm::vec3(cos(theta2), sin(theta2), 0) * radius;
			_vertices.push_back({ p1, color });
			_vertices.push_back({ p2, color });

			// XZ circle
			glm::vec3 q1 = center + glm::vec3(cos(theta1), 0, sin(theta1)) * radius;
			glm::vec3 q2 = center + glm::vec3(cos(theta2), 0, sin(theta2)) * radius;
			_vertices.push_back({ q1, color });
			_vertices.push_back({ q2, color });

			// YZ circle
			glm::vec3 r1 = center + glm::vec3(0, cos(theta1), sin(theta1)) * radius;
			glm::vec3 r2 = center + glm::vec3(0, cos(theta2), sin(theta2)) * radius;
			_vertices.push_back({ r1, color });
			_vertices.push_back({ r2, color });
		}
	}

	void DebugRenderer::GenerateCapsule(const glm::vec3& center, float height, float radius, const glm::vec3& color)
	{
		const int segments = 12;
		const float halfHeight = height * 0.5f;

		glm::vec3 topCenter = center + glm::vec3(0, halfHeight, 0);
		glm::vec3 bottomCenter = center - glm::vec3(0, halfHeight, 0);

		for (int i = 0; i <= segments / 2; ++i) {
			float phi1 = glm::pi<float>() * (float)i / (segments);
			float phi2 = glm::pi<float>() * (float)(i + 1) / (segments);
			for (int j = 0; j < segments; ++j) {
				float theta1 = 2.0f * glm::pi<float>() * j / segments;
				float theta2 = 2.0f * glm::pi<float>() * (j + 1) / segments;

				glm::vec3 p1 = topCenter + glm::vec3(
					radius * sin(phi1) * cos(theta1),
					radius * cos(phi1),
					radius * sin(phi1) * sin(theta1));

				glm::vec3 p2 = topCenter + glm::vec3(
					radius * sin(phi2) * cos(theta1),
					radius * cos(phi2),
					radius * sin(phi2) * sin(theta1));

				glm::vec3 p3 = topCenter + glm::vec3(
					radius * sin(phi1) * cos(theta2),
					radius * cos(phi1),
					radius * sin(phi1) * sin(theta2));

				_vertices.push_back({ p1, color }); _vertices.push_back({ p2, color });
				_vertices.push_back({ p1, color }); _vertices.push_back({ p3, color });
			}
		}

		for (int i = 0; i <= segments / 2; ++i) {
			float phi1 = glm::pi<float>() * (float)i / (segments);
			float phi2 = glm::pi<float>() * (float)(i + 1) / (segments);
			for (int j = 0; j < segments; ++j) {
				float theta1 = 2.0f * glm::pi<float>() * j / segments;
				float theta2 = 2.0f * glm::pi<float>() * (j + 1) / segments;

				glm::vec3 p1 = bottomCenter + glm::vec3(
					radius * sin(phi1) * cos(theta1),
					-radius * cos(phi1),
					radius * sin(phi1) * sin(theta1));

				glm::vec3 p2 = bottomCenter + glm::vec3(
					radius * sin(phi2) * cos(theta1),
					-radius * cos(phi2),
					radius * sin(phi2) * sin(theta1));

				glm::vec3 p3 = bottomCenter + glm::vec3(
					radius * sin(phi1) * cos(theta2),
					-radius * cos(phi1),
					radius * sin(phi1) * sin(theta2));

				_vertices.push_back({ p1, color }); _vertices.push_back({ p2, color });
				_vertices.push_back({ p1, color }); _vertices.push_back({ p3, color });
			}
		}

		for (int i = 0; i < segments; ++i) {
			float theta = glm::two_pi<float>() * i / segments;
			glm::vec3 offset = glm::vec3(cos(theta), 0, sin(theta)) * radius;

			glm::vec3 top = topCenter - glm::vec3(0, radius, 0) + offset;
			glm::vec3 bottom = bottomCenter + glm::vec3(0, radius, 0) + offset;

			_vertices.push_back({ top, color });
			_vertices.push_back({ bottom, color });
		}
	}
}