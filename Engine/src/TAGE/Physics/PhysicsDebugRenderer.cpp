#include "tagepch.h"
#include "TARE/TARE.h"
#include "Raycaster.h"
#include "PhysicsDebugRenderer.h"
#include "TARE/Debug/DebugRenderer.h"
#include "Raycaster.h"

namespace TAGE::Physics {
    void PhysicsDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        m_Lines.push_back({
            glm::vec3(from.x(), from.y(), from.z()),
            glm::vec3(to.x(), to.y(), to.z()),
            glm::vec3(color.x(), color.y(), color.z()),
            RayDrawType::FOR_FRAME,
            0.0f
        });
    }

    void PhysicsDebugRenderer::drawLineForSeconds(const btVector3& from, const btVector3& to, const btVector3& color, float duration)
    {
        m_Lines.push_back({
            glm::vec3(from.x(), from.y(), from.z()),
            glm::vec3(to.x(), to.y(), to.z()),
            glm::vec3(color.x(), color.y(), color.z()),
            RayDrawType::FOR_DURATION,
            duration
        });
    }

    void PhysicsDebugRenderer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB,
        btScalar distance, int lifeTime, const btVector3& color)
    {
        btVector3 to = pointOnB + normalOnB * distance;
        drawLine(pointOnB, to, color);
    }

    void PhysicsDebugRenderer::reportErrorWarning(const char* warningString)
    {
        LOG_WARN("Bullet Debug: {0}", warningString);
    }

    void PhysicsDebugRenderer::BeginFrame(float dt)
    {
        auto it = m_Lines.begin();
        while (it != m_Lines.end()) {
            if (it->drawType == RayDrawType::FOR_DURATION) {
                it->time -= dt;
                if (it->time <= 0.0f)
                    it = m_Lines.erase(it);
                else
                    ++it;
            }
            else if (it->drawType == RayDrawType::FOR_FRAME) {
                it = m_Lines.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void PhysicsDebugRenderer::Render(float dt, const glm::mat4& viewproj)
    {
         TARE::Debug::DebugRenderer::Get().BeginFrame();
        for (const auto& line : m_Lines)
             TARE::Debug::DebugRenderer::Get().DrawLine(line.from, line.to, line.color, line.time);
         TARE::Debug::DebugRenderer::Get().Render(viewproj);
         TARE::Debug::DebugRenderer::Get().EndFrame(dt);
    }

    void PhysicsDebugRenderer::Flush(float dt, const glm::mat4& viewproj)
    {
         TARE::Debug::DebugRenderer::Get().BeginFrame();
        auto it = m_Lines.begin();
        while (it != m_Lines.end()) {
            if (it->drawType == RayDrawType::FOR_DURATION)
            {
                 TARE::Debug::DebugRenderer::Get().DrawLine(it->from, it->to, it->color, it->time);
                it->time -= dt;
                if (it->time <= 0.0f)
                    it = m_Lines.erase(it);
                else
                    ++it;
            }
            else if (it->drawType == RayDrawType::FOR_FRAME)
            {
                 TARE::Debug::DebugRenderer::Get().DrawLine(it->from, it->to, it->color);
                it = m_Lines.erase(it);
            }
            else
            {
                ++it;
            }
        }
         TARE::Debug::DebugRenderer::Get().EndFrame(dt);
         TARE::Debug::DebugRenderer::Get().Render(viewproj);
    }
}