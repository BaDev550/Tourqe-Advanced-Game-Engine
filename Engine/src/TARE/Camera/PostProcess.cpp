#include "tagepch.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TARE/Common/RenderCommands.h"
#include "PostProcess.h"

namespace TARE {
	PostProcess::PostProcess(int width, int height, PostProcessSettings settings)
	{
		_settings = settings;
		FramebufferSpecification spec(
			FramebufferAttachmentSpecification({
				FramebufferTextureFormat(FramebufferTextureFormat::RGBA)
				}), _settings.AA_Samples, width, height
		);
		_framebuffer = Framebuffer::Create(spec);
		_postProcessShader = ShaderLibrary::Add("PostProcces", "shaders/Screen/postprocess_vertex", "shaders/Screen/postprocess_fragment");
	}

	void PostProcess::Render(const TAGE::MEM::Ref<Framebuffer>& source)
	{
		_framebuffer->Bind();
		_framebuffer->Clear(0, 0);

		_postProcessShader->Use();
		_postProcessShader->SetUniform("u_Exposure", _settings.Exposure);

		RenderCommand::BindTextureFromID(source->GetColorAttachment(0), 3);
		_postProcessShader->SetUniform("u_ScreenTexture", 3);

		RenderCommand::DrawFullScreenQuad();
		_framebuffer->Unbind();
	}
}