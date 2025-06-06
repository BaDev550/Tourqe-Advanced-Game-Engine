#pragma once

#include "TAGE/Renderer/Texture/Texture.h"
#include "TAGE/Renderer/Shader/Shader.h"

namespace TAGE::Renderer {
	class Skybox
	{
        std::vector<std::string> faces
        {
            "../Engine/assets/textures/skybox/right.jpg",
            "../Engine/assets/textures/skybox/left.jpg",
            "../Engine/assets/textures/skybox/top.jpg",
            "../Engine/assets/textures/skybox/bottom.jpg",
            "../Engine/assets/textures/skybox/front.jpg",
            "../Engine/assets/textures/skybox/back.jpg"
        };
	public:
        Skybox();

        void DrawSkybox(const glm::mat4& view, const glm::mat4& projection);
    private:
        MEM::Ref<TextureCubeMap> _CubeMap;
        MEM::Ref<Shader> _SkyBoxShader;
        uint _VAO, _VBO, EBO;
	};
}