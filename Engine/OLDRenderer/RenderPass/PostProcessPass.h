#pragma once

#include "RenderPass.h"
#include "TAGE/Common/TDefines.h"
#include "TAGE/Utilities/Memory.h"
#include "TAGE/Renderer/Framebuffer/Framebuffer.h"
#include "TAGE/Renderer/Shader/ShaderLibrary.h"
#include "TAGE/Renderer/Texture/Texture.h"

namespace TAGE::Renderer {
	class PostProcessPass : public RenderPass
	{
	public:
		PostProcessPass(int width, int height)
			: _Framebuffer(MEM::MakeScope<Framebuffer>(width, height, true))
		{
			_PostProcessShader = ShaderLibrary::Add("PostProcess", "../Engine/shaders/Screen/postprocess_vertex.glsl", "../Engine/shaders/Screen/postprocess_fragment.glsl");
		}

		virtual void Begin() override {
			_Framebuffer->Bind();
			_Framebuffer->Clear();
		}
		
		virtual void End() override {
			_Framebuffer->Unbind();
		}

		virtual void Render() override {
			_Framebuffer->RenderToScreen("LightDeferredShader");
			//"LightDeferredShader"
		}

		MEM::Ref<Texture2D> GetColorAttachment() const {
			return MEM::MakeRef<Texture2D>(_Framebuffer->GetColorAttachmentID(), _Framebuffer->GetWidth(), _Framebuffer->GetHeight());
		}

		void SetInputTexture(uint ID) {
			_Framebuffer->SetInput(ID);
		}

		virtual Framebuffer* GetBuffer() { return _Framebuffer.get(); };
	private:
		MEM::Scope<Framebuffer> _Framebuffer;
		MEM::Ref<Shader> _PostProcessShader;
	};
}