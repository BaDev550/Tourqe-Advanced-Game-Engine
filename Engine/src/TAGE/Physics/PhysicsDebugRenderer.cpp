#include "tagepch.h"
#include "TARE/TARE.h"
#include "Raycaster.h"
#include "PhysicsDebugRenderer.h"
#include "TARE/Debug/DebugRenderer.h"
#include "Raycaster.h"

namespace TAGE::Physics {
    void PhysicsDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        TARE::Debug::DebugRenderer::Get().DrawLine(
            { from.x(), from.y(), from.z() },
            { to.x(), to.y(), to.z() },
            { color.x(), color.y(), color.z() },
            0.0f
        );
    }

    void PhysicsDebugRenderer::drawLineForSeconds(const btVector3& from, const btVector3& to, const btVector3& color, float duration)
    {
        TARE::Debug::DebugRenderer::Get().DrawLine(
            { from.x(), from.y(), from.z() },
            { to.x(), to.y(), to.z() },
            { color.x(), color.y(), color.z() },
            duration
        );
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
}