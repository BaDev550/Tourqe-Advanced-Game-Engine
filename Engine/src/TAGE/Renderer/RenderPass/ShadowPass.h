#pragma once

#include "RenderPass.h"
#include "TAGE/Common/TDefines.h"
#include "TAGE/Utilities/Memory.h"
#include "TAGE/Renderer/Framebuffer/Framebuffer.h"
#include "TAGE/Renderer/Shader/ShaderLibrary.h"
#include "TAGE/Renderer/Texture/Texture.h"
#include "TAGE/Renderer/Shadow/ShadowMap.h"

namespace TAGE::Renderer {
	class ShadowPass : public RenderPass
	{
	public:
		ShadowPass(int width, int height)
		{
			_Framebuffer = MEM::MakeScope<ShadowMap>(width, height);
			_ShadowShader = ShaderLibrary::Add("ShadowShader", "../Engine/shaders/shadow_vertex.glsl", "../Engine/shaders/shadow_fragment.glsl");
		}

		virtual void Begin() override {
			_Framebuffer->Bind();
		}

		virtual void End() override {
			_Framebuffer->Unbind();
		}

		virtual void Render() override {
		}

		void SetLightProjectionMatrix(const glm::mat4& lightViewProj)
		{
			_ShadowShader->Use();
			_ShadowShader->SetUniform("u_LightSpaceMatrix", lightViewProj);
		}

		void BindShadowMap() const {
			_Framebuffer->BindDepthTexture(SHADOW_MAP_TEXTURE_SLOT);
		}

		virtual Framebuffer* GetBuffer() { return nullptr; };
		ShadowMap* GetShadowMap() { return _Framebuffer.get(); }
	private:
		MEM::Scope<ShadowMap> _Framebuffer;
		MEM::Ref<Shader> _ShadowShader;
	};
}