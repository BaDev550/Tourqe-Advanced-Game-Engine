#include "tagepch.h"
#include "DeferredRendering.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE {
	DeferredRendering::DeferredRendering(int width, int height)
	{
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
		_GBuffer->Clear(6, -1);

		_GBufferShader->Use();
		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(0), 0);
		_GBufferShader->SetUniform("gPosition", 0);
		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(0), 1);
		_GBufferShader->SetUniform("gNormal", 1);
		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(0), 2);
		_GBufferShader->SetUniform("gAlbedo", 2);
	}

	void DeferredRendering::RenderLightingPass(std::vector<Light>& lights, const glm::vec3& cameraPos) const
	{
		_LightingBuffer->Bind();
		RenderCommand::Enable(BLEND_TEST);
		RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		RenderCommand::Clear(COLOR);

		_LightShader->Use();
		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(0), 1);
		_LightShader->SetUniform("u_gPos", 1);

		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(1), 2);
		_LightShader->SetUniform("u_gNorm", 2);

		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(2), 3);
		_LightShader->SetUniform("u_gAlb", 3);

		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(3), 4);
		_LightShader->SetUniform("u_gMetallic", 4);

		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(4), 5);
		_LightShader->SetUniform("u_gRoughness", 5);

		RenderCommand::BindTextureFromID(_GBuffer->GetColorAttachment(5), 6);
		_LightShader->SetUniform("u_gAO", 6);

		RenderCommand::BindTextureFromID(_GBuffer->GetDepthAttachment(), 7);
		_LightShader->SetUniform("u_gDepth", 7);

		RenderCommand::BindTextureFromID(_LightingBuffer->GetColorAttachment(), 8);
		_LightShader->SetUniform("u_SceneColor", 8);

		RenderCommand::BindTextureFromID(_GIBuffer->GetColorAttachment(0), 9);
		_LightShader->SetUniform("u_PrevGI", 9);

		RenderCommand::DrawFullScreenQuad();
		_LightingBuffer->Blit(_GIBuffer);
		_LightingBuffer->Unbind();
	}

	void DeferredRendering::CreateGBuffer(int width, int height)
	{
		FramebufferSpecification spec(
			FramebufferAttachmentSpecification({
					FramebufferTextureFormat(FramebufferTextureFormat::RGBA16F), // Position
					FramebufferTextureFormat(FramebufferTextureFormat::RGBA16F), // Normal
					FramebufferTextureFormat(FramebufferTextureFormat::RGBA8),   // Albedo
					FramebufferTextureFormat(FramebufferTextureFormat::R16F),    // Metallic
					FramebufferTextureFormat(FramebufferTextureFormat::R16F),    // Roughness
					FramebufferTextureFormat(FramebufferTextureFormat::R8),      // AO
					FramebufferTextureFormat(FramebufferTextureFormat::RED_INTEGER), // Entity ID's
					FramebufferTextureFormat(FramebufferTextureFormat::DEPTH24), // Depth
				}),
			1, width, height
		);
		_GBuffer = Framebuffer::Create(spec);
		_GBufferShader = ShaderLibrary::Add("GBufferShader", "shaders/Deferred/gbuffer_vertex", "shaders/Deferred/gbuffer_fragment");

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
		_LightShader = ShaderLibrary::Add("LightShader", "shaders/Deferred/light_vertex", "shaders/Deferred/light_fragment");
	}
}