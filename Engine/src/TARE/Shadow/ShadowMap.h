#pragma once
#include "TARE/Buffers/Framebuffer.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TARE/Camera/Camera.h"

namespace TARE {
	class ShadowMap
	{
    public:
        ShadowMap(int width, int height);
        ~ShadowMap() = default;

        void BeginRender(const glm::mat4& lightViewProj);
        void EndRender();

        void BindTexture(uint slot = 0) const;
		void UseShader() const { _depthShader->Use(); }
		uint GetTextureID() const { return _depthFBO->GetDepthAttachment(); }
        const glm::mat4& GetLightSpaceMatrix() const { return _lightSpaceMatrix; }

        void SetLightDirection(const glm::vec3& dir);
        void SetOrthoBounds(float left, float right, float bottom, float top, float near, float far);

    private:
        TAGE::MEM::Ref<Framebuffer> _depthFBO;
        TAGE::MEM::Ref<Shader> _depthShader;

        glm::mat4 _lightSpaceMatrix;
        glm::vec3 _lightDir;
        glm::mat4 _lightView;
        glm::mat4 _lightProj;

        void UpdateMatrices();
	};
}

