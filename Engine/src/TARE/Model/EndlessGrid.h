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
	class EndlessGrid
	{
    public:
        EndlessGrid();

        void Render();
    private:
        TAGE::MEM::Ref<VertexArrayObject> _VAO;
        TAGE::MEM::Ref<Shader> _GridShader;
        void InitGridBuffers();
	};
}