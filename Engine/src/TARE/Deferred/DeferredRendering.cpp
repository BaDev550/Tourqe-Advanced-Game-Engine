#include "tagepch.h"
#include "DeferredRendering.h"
#include "TARE/Common/RenderCommands.h"
#include "TAGE/Input/Input.h"

namespace TARE {
	DeferredRendering::DeferredRendering(int width, int height)
	{
		_FinalShader = ShaderLibrary::Add("FinalShader", "../Engine/shaders/Screen/postprocess_vertex", "../Engine/shaders/Screen/postprocess_fragment");
		CreateGBuffer(width, height);
	}
	DeferredRendering::~DeferredRendering()
	{
	}

	void DeferredRendering::RenderGeometryPass(const TAGE::MEM::Ref<Camera>& camera) const
	{
		_GBuffer->Bind();
		RenderCommand::Disable(BLEND_TEST);
		RenderCommand::Clear(COLOR_DEPTH_STENCIL);
		_GBuffer->Clear(3, -1);

		_GBufferShader->Use();
		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(0), 0);
		_GBufferShader->SetUniform("gPosition", 0);
		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(0), 1);
		_GBufferShader->SetUniform("gNormal", 1);
		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(0), 2);
		_GBufferShader->SetUniform("gAlbedo", 2);
		_GBufferShader->SetUniform("u_View", camera->GetViewMatrix());
		_GBufferShader->SetUniform("u_Projection", camera->GetProjectionMatrix());
	}

	void DeferredRendering::RenderLightingPass(std::vector<Light>& lights, const glm::vec3& cameraPos) const
	{
		_LightingBuffer->Bind();
		RenderCommand::Enable(BLEND_TEST);
		RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		RenderCommand::Clear(COLOR);
		_LightingBuffer->Clear(0, 0);

		_LightShader->Use();
		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(0), 1);
		_LightShader->SetUniform("u_gPos", 1);

		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(1), 2);
		_LightShader->SetUniform("u_gNorm", 2);

		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(2), 3);
		_LightShader->SetUniform("u_gAlb", 3);

		RenderCommand::BindTextureFromID(_GBuffer->GetDepthAttachment(), 4);
		_LightShader->SetUniform("u_gDepth", 4);

		RenderCommand::BindTextureFromID(_LightingBuffer->GetColorAttachment(0), 5);
		_LightShader->SetUniform("u_SceneColor", 5);

		RenderCommand::BindTextureFromID(_GIBuffer->GetColorAttachment(0), 6);
		_LightShader->SetUniform("u_PrevGI", 6);

		_LightShader->SetUniform("u_CameraPos", cameraPos);
		_LightShader->SetUniform("u_ScreenWidth", (float)_GBuffer->GetSpecification().Width);
		_LightShader->SetUniform("u_ScreenHeight", (float)_GBuffer->GetSpecification().Height);

		RenderCommand::DrawFullScreenQuad();

		_LightingBuffer->Blit(_GIBuffer);
	}

	void DeferredRendering::BlitToScreen() const
	{
		_LightingBuffer->Unbind();
		_FinalShader->Use();
		RenderCommand::BindTextureFromID(_LightingBuffer->GetColorAttachment(0), 0);
		_FinalShader->SetUniform("u_ScreenTexture", 0);
		RenderCommand::DrawFullScreenQuad();
	}

	void DeferredRendering::CreateGBuffer(int width, int height)
	{
		FramebufferSpecification spec(
			FramebufferAttachmentSpecification({
					FramebufferTextureFormat(FramebufferTextureFormat::RGBA16F), // Position
					FramebufferTextureFormat(FramebufferTextureFormat::RGBA16F), // Normal
					FramebufferTextureFormat(FramebufferTextureFormat::RGBA8), // Albedo
					FramebufferTextureFormat(FramebufferTextureFormat::RED_INTEGER), // Entity ID's
					FramebufferTextureFormat(FramebufferTextureFormat::DEPTH32F) // Depth
				}),
			1, width, height
		);
		_GBuffer = Framebuffer::Create(spec);
		_GBufferShader = ShaderLibrary::Add("GBufferShader", "../Engine/shaders/Deferred/gbuffer_vertex", "../Engine/shaders/Deferred/gbuffer_fragment");

		FramebufferSpecification GIspec(
			FramebufferAttachmentSpecification({
					FramebufferTextureFormat(FramebufferTextureFormat::RGBA16F) // Prev GI Result
				}),
				1, width, height
				);
		_GIBuffer = Framebuffer::Create(GIspec);

		CreateLightingBuffer();
	}

	void DeferredRendering::CreateLightingBuffer()
	{
		FramebufferSpecification spec(
			FramebufferAttachmentSpecification({
					FramebufferTextureFormat::RGBA // Lighting
			}),
			1, _GBuffer->GetSpecification().Width, _GBuffer->GetSpecification().Height
		);
		_LightingBuffer = Framebuffer::Create(spec);
		_LightShader = ShaderLibrary::Add("LightShader", "../Engine/shaders/Deferred/light_vertex", "../Engine/shaders/Deferred/light_fragment");
	}
}