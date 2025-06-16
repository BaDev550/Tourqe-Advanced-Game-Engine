#pragma once

#include "TAGE/Utilities/Memory.h"
#include "TAGE/Renderer/Camera/Camera.h"
#include "TAGE/Common/TDefines.h"
#include "glm/glm.hpp"
#include <vector>

namespace TAGE::Renderer
{
	class EndlessGrid
	{
    public:
        EndlessGrid();
        ~EndlessGrid();

        void Render(const MEM::Ref<Camera>& camera);

    private:
        unsigned int _VAO, _VBO;
        void InitGridBuffers();
	};
}