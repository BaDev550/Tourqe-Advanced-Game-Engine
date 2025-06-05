#pragma once

#include "RenderPass.h"
#include "TAGE/Common/TDefines.h"
#include "TAGE/Utilities/Memory.h"
#include "TAGE/Renderer/Framebuffer/Framebuffer.h"
#include "TAGE/Renderer/Shader/ShaderLibrary.h"
#include "TAGE/Renderer/Texture/Texture2D.h"

namespace TAGE::Renderer {
	class BloomPass : public RenderPass
	{
	public:
		BloomPass(int width, int height)
			: _Framebuffer(MEM::MakeScope<Framebuffer>(width, height, true))
		{
			_BloomShader = ShaderLibrary::Add("Bloom", "../Engine/shaders/bloom_vertex.glsl", "../Engine/shaders/bloom_fragment.glsl");
		}

		virtual void Begin() override {
			_Framebuffer->Bind();
			_Framebuffer->Clear();
		}

		virtual void End() override {
			_Framebuffer->Unbind();
		}

		virtual void Render() override {
			_BloomShader->Use();
			_BloomShader->SetUniform("u_Scene", 1);
			_BloomShader->SetUniform("u_Threshold", 1.0f);
			_Framebuffer->RenderToScreen("Bloom");
		}

		void ExtractBloom(const MEM::Ref<Texture2D>& sceneTexture) {
			_Framebuffer->Bind();
			_Framebuffer->Clear();

			_BloomShader->Use();
			sceneTexture->Bind(0);
			_BloomShader->SetUniform("u_Scene", 0);
			_BloomShader->SetUniform("u_Threshold", 1.0f);

			_Framebuffer->RenderToScreen("Bloom");

			sceneTexture->Unbind();
			_Framebuffer->Unbind();
		}

		virtual Framebuffer* GetBuffer() { return _Framebuffer.get(); };
	private:
		MEM::Scope<Framebuffer> _Framebuffer;
		MEM::Ref<Shader> _BloomShader;
	};
}