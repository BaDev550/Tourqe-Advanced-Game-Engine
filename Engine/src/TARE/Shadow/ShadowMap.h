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

        void BeginRender(const TAGE::MEM::Ref<Camera>& cam, const glm::vec3& lightDir);
        void EndRender();

        void BindTexture(uint slot = 0) const;
		void UseShader() const { _depthShader->Use(); }
		uint GetTextureID() const { return _depthFBO->GetDepthAttachment(); }

		std::vector<float>& GetShadowCascadeLevels() { return _shadowCascadeLevels; }
		TAGE::MEM::Ref<Framebuffer>& GetFramebuffer() { return _depthFBO; }
    private:
        TAGE::MEM::Ref<Framebuffer> _depthFBO;
        TAGE::MEM::Ref<Shader> _depthShader;
		std::vector<float> _shadowCascadeLevels = { 500.0f / 50.0f, 500.0f / 25.0f, 500.0f / 10.0f, 500.0f / 2.0f };
		TAGE::MEM::Weak<Camera> _camera;
		int width, height;

        std::vector<glm::mat4> getLightSpaceMatrices(const glm::vec3& lightDir) const;
        glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane, const glm::vec3& lightDir) const;
	};
}

