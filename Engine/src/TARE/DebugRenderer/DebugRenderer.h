#pragma once

#include "TARE/Buffers/VertexArrayBuffer.h"
#include "TAGE/Utilities/Memory.h"
#include "TARE/Shader/Shader.h"
#include "TAGE/Common/TTypes.h"

// Debug Renderer
namespace TARE::Debug {
	class DebugRenderer
	{
	public:
        void BeginFrame();
        void DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float duration = 0.0f);
        void DrawSphere(const glm::vec3& center, float radius, const glm::vec3& color, float duration = 0.0f);
        void DrawCapsule(const glm::vec3& center, float height, float radius, const glm::vec3& color, float duration = 0.0f);
        void EndFrame(float deltaTime);
        void Render(const glm::mat4& viewProj);
        void Init();

        static DebugRenderer& Get()
        {
            static DebugRenderer instance;
            return instance;
        }
    private:
        DebugRenderer() = default;
        ~DebugRenderer() = default;
        DebugRenderer(const DebugRenderer&) = delete;
        DebugRenderer& operator=(const DebugRenderer&) = delete;

        struct DebugDrawCommand {
            DebugPrimitiveType Type;
            float TimeRemaining;
            glm::vec3 Color;

            glm::vec3 P1, P2;
            float Radius = 1.0f;
            float Height = 1.0f;
        };

        std::vector<DebugDrawCommand> _commands;
        std::vector<DebugVertexData> _vertices;

        TAGE::MEM::Ref<VertexBufferObject> _VBO = nullptr;
        TAGE::MEM::Ref<VertexArrayObject> _VAO = nullptr;

        TAGE::MEM::Ref<Shader> _debugShader;

        bool _initialized = false;

        void GenerateGeometry();
        void GenerateSphere(const glm::vec3& center, float radius, const glm::vec3& color);
        void GenerateCapsule(const glm::vec3& center, float height, float radius, const glm::vec3& color);
	};
}