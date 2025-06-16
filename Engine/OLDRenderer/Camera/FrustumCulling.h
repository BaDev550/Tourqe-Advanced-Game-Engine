#pragma once

#include "glm/glm.hpp"

namespace TAGE::Frustum {
    struct Plane {
        glm::vec3 normal;
        float d;
    };

    struct Frustum {
        Plane planes[6];
    };

    Frustum ExtractFrustumPlanes(const glm::mat4& viewProj) {
        Frustum frustum;

        // Left
        frustum.planes[0].normal.x = viewProj[0][3] + viewProj[0][0];
        frustum.planes[0].normal.y = viewProj[1][3] + viewProj[1][0];
        frustum.planes[0].normal.z = viewProj[2][3] + viewProj[2][0];
        frustum.planes[0].d = viewProj[3][3] + viewProj[3][0];

        // Right
        frustum.planes[1].normal.x = viewProj[0][3] - viewProj[0][0];
        frustum.planes[1].normal.y = viewProj[1][3] - viewProj[1][0];
        frustum.planes[1].normal.z = viewProj[2][3] - viewProj[2][0];
        frustum.planes[1].d = viewProj[3][3] - viewProj[3][0];

        // Bottom
        frustum.planes[2].normal.x = viewProj[0][3] + viewProj[0][1];
        frustum.planes[2].normal.y = viewProj[1][3] + viewProj[1][1];
        frustum.planes[2].normal.z = viewProj[2][3] + viewProj[2][1];
        frustum.planes[2].d = viewProj[3][3] + viewProj[3][1];

        // Top
        frustum.planes[3].normal.x = viewProj[0][3] - viewProj[0][1];
        frustum.planes[3].normal.y = viewProj[1][3] - viewProj[1][1];
        frustum.planes[3].normal.z = viewProj[2][3] - viewProj[2][1];
        frustum.planes[3].d = viewProj[3][3] - viewProj[3][1];

        // Near
        frustum.planes[4].normal.x = viewProj[0][3] + viewProj[0][2];
        frustum.planes[4].normal.y = viewProj[1][3] + viewProj[1][2];
        frustum.planes[4].normal.z = viewProj[2][3] + viewProj[2][2];
        frustum.planes[4].d = viewProj[3][3] + viewProj[3][2];

        // Far
        frustum.planes[5].normal.x = viewProj[0][3] - viewProj[0][2];
        frustum.planes[5].normal.y = viewProj[1][3] - viewProj[1][2];
        frustum.planes[5].normal.z = viewProj[2][3] - viewProj[2][2];
        frustum.planes[5].d = viewProj[3][3] - viewProj[3][2];

        for (int i = 0; i < 6; ++i) {
            float length = glm::length(frustum.planes[i].normal);
            frustum.planes[i].normal /= length;
            frustum.planes[i].d /= length;
        }
        return frustum;
    }

    bool IsSphereInFrustum(const glm::vec3& center, float radius, const Frustum& frustum) {
        for (int i = 0; i < 6; ++i) {
            float distance = glm::dot(frustum.planes[i].normal, center) + frustum.planes[i].d;
            if (distance < -radius)
                return false;
        }
        return true;
    }
}
