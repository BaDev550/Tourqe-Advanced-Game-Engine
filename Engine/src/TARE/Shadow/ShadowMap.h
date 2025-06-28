#pragma once
#include "TARE/Buffers/Framebuffer.h"
#include "TAGE/Utilities/Memory.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TARE/Camera/Camera.h"
#include "TARE/Texture/Texture.h"

namespace TARE {
	class ShadowMap
	{
    public:
        ShadowMap(int width, int height);
        ~ShadowMap() = default;

        void BeginRender(const glm::mat4& lightViewProj);
        void EndRender();
        void BeginPointLightShadowPass(const glm::vec3& lightPos);
        void SetFaceViewProjection(int faceIndex, const glm::mat4& vpMatrix);
        void EndPointLightShadowPass();

        void BindTexture(uint slot = 0) const;
		void UseShader() const { _depthShader->Use(); }
		uint GetTextureID() const { return _depthFBO->GetDepthAttachment(); }

		TAGE::MEM::Ref<Framebuffer>& GetFramebuffer() { return _depthFBO; }
    private:
        TAGE::MEM::Ref<Framebuffer> _depthFBO;
        TAGE::MEM::Ref<Shader> _depthShader;
        TAGE::MEM::Ref<Shader> _pointLightDepthShader;
        TAGE::MEM::Ref<CubemapTexture> _pointDepthCubemap;
	};
}

