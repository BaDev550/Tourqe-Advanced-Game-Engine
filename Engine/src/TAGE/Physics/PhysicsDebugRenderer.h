#pragma once

#include <btBulletDynamicsCommon.h>
#include "TAGE/Common/TTypes.h"
#include "TARE/Debug/DebugRenderer.h"
#include <glm/glm.hpp>

namespace TAGE::Physics {
    class PhysicsDebugRenderer : public btIDebugDraw
    {
    public:
        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
        virtual void drawLineForSeconds(const btVector3& from, const btVector3& to, const btVector3& color, float duration);
        virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
        virtual void reportErrorWarning(const char* warningString) override;
        virtual void setDebugMode(int debugMode) override { m_DebugMode = debugMode; }
        virtual void draw3dText(const btVector3& location, const char* textString) override {}
        int getDebugMode() const override { return m_DebugMode; }

        void BeginFrame(float dt);
        void Render(float dt, const glm::mat4& viewproj);

        void Flush(float dt, const glm::mat4& viewproj);
    private:
        std::vector<RaycastLine> m_Lines;
        int m_DebugMode = DBG_DrawWireframe | DBG_DrawContactPoints | DBG_DrawAabb;
    };
}