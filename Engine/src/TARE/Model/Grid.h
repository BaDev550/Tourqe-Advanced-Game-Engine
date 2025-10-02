#pragma once

#include "TAGE/Utilities/Memory.h"
#include "TARE/Camera/Camera.h"
#include "TAGE/Common/TDefines.h"
#include "TARE/Buffers/VertexArrayBuffer.h"
#include "TARE/Shader/Shader.h"
#include "glm/glm.hpp"
#include <vector>

namespace TARE
{
    class Grid
    {
    public:
        Grid();
        static void Create();
        static void Draw();
    private:
        static inline TAGE::MEM::Ref<VertexArrayObject> _VAO = nullptr;
        static void InitGridBuffers();
    };
}