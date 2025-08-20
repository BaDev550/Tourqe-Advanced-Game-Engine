#include "tagepch.h"
#include "DebugLightRenderer.h"
#include "TARE/Common/RenderCommands.h"
#include "TARE/Shader/ShaderLibrary.h"

namespace TARE::Debug {
    void DebugLightRenderer::Render(const glm::mat4& viewproj, const glm::vec3& position, float radius, int segments, const glm::vec3& color, bool wireframe)
    {
        _shader = ShaderLibrary::Get("DebugShader");
        if (_shader) {
            _shader->Use();

            if (!_VAO)
                GenerateGeometry(radius, segments);

            _VAO->Bind();
            glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, position);

            _shader->SetUniform("u_Color", color);
            _shader->SetUniform("u_Model", model);
			_shader->SetUniform("u_ViewProj", viewproj);

            if (wireframe)
                TARE::RenderCommand::SetWireframe(true);
            
            TARE::RenderCommand::DrawIndexed(_VAO);

            if (wireframe)
                TARE::RenderCommand::SetWireframe(false);

            _VAO->Unbind();
        }
    }

    void DebugLightRenderer::GenerateSpotLightCone(float radius, float height, int segments)
	{
        _vertices.clear();
        _indices.clear();

        _vertices.push_back({ {0, 0, 0} });
        for (int i = 0; i < segments; ++i) {
            float theta = 2.0f * glm::pi<float>() * i / segments;
            float x = radius * cos(theta);
            float z = radius * sin(theta);
            _vertices.push_back({ {x, -height, z} });
        }

        for (int i = 0; i < segments; ++i) {
            int next = (i + 1) % segments;
            _indices.push_back(0);       
            _indices.push_back(1 + i);   
            _indices.push_back(1 + next);
        }

        for (int i = 1; i < segments - 1; ++i) {
            _indices.push_back(1);         
            _indices.push_back(1 + i);     
            _indices.push_back(1 + i + 1);  
        }
	}

	void DebugLightRenderer::GeneratePointLightSphere(float radius, int latSegments, int lonSegments)
	{
        _vertices.clear();
        _indices.clear();

        for (int lat = 0; lat <= latSegments; ++lat) {
            float phi = glm::pi<float>() * lat / latSegments;
            for (int lon = 0; lon <= lonSegments; ++lon) {
                float theta = 2.0f * glm::pi<float>() * lon / lonSegments;
                float x = radius * sin(phi) * cos(theta);
                float y = radius * cos(phi);
                float z = radius * sin(phi) * sin(theta);
                _vertices.push_back({ {x, y, z} });
            }
        }

        for (int lat = 0; lat < latSegments; ++lat) {
            for (int lon = 0; lon < lonSegments; ++lon) {
                int current = lat * (lonSegments + 1) + lon;
                int next = current + lonSegments + 1;

                _indices.push_back(current);
                _indices.push_back(next);
                _indices.push_back(current + 1);

                _indices.push_back(current + 1);
                _indices.push_back(next);
                _indices.push_back(next + 1);
            }
        }
	}

	void DebugLightRenderer::GenerateSunOutline(float size)
	{
        _vertices = {
            {{-size, 0, -size}},
            {{ size, 0, -size}},
            {{ size, 0,  size}},
            {{-size, 0,  size}}
        };

        _indices = { 0,1, 1,2, 2,3, 3,0 };
	}

	void DebugLightRenderer::GenerateGeometry(float radius, int segments)
	{
        switch (_type)
        {
        case LightType::POINT: GeneratePointLightSphere(radius, segments, segments); break;
        case LightType::SPOT: GenerateSpotLightCone(radius, 1.0f, segments); break;
        case LightType::DIRECTIONAL: GenerateSunOutline(1.0f); break;
        default:
            break;
        }

        TAGE::MEM::Ref<VertexBufferObject> _VBO;
        _VBO = VertexBufferObject::Create(_vertices.data(), _vertices.size() * sizeof(DebugVertexData));

        BufferLayout layout = {
            { ShaderDataType::VEC3, "aPos" }
        };

        _VAO = VertexArrayObject::Create();
        _VBO->SetLayout(layout);
        _VAO->AddVertexBuffer(_VBO);

        TAGE::MEM::Ref<ElementBufferObject> _EBO;
        _EBO = ElementBufferObject::Create(_indices.data(), _indices.size());
        _VAO->SetIndexBuffer(_EBO);
	}
}