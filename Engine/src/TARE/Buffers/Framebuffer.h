#pragma once
#include "TAGE/Common/TDefines.h"
#include "TARE/Texture/Texture.h"

namespace TARE {
	struct FramebufferTextureSpecification {
		TextureFormat format = TextureFormat::RGBA8;

		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(TextureFormat fmt) : format(fmt) {}
	};

	struct FramebufferAttachmentSpecification {
		std::vector<FramebufferTextureSpecification> Attachments;

		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) : Attachments(attachments) {}
	};

	struct FramebufferSpecification
	{
		uint Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint Samples = 1;
		uint DepthArrayLayers = 1;

		bool SwapChainTarget = false;
		FramebufferSpecification(FramebufferAttachmentSpecification attachments, uint samples, uint width, uint height, uint depthArrayLayers = 1)
			: Attachments(attachments), Samples(samples), Width(width), Height(height), DepthArrayLayers(depthArrayLayers) {
		}
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() {}
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Resize(uint width, uint height) = 0;
		virtual int Read(uint attachment, int x, int y) const = 0;
		virtual void ClearAttachment(uint attachment, int value) = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;
		virtual void AttachCubemapFace(const TAGE::MEM::Ref<TextureCube>& cubemap, uint faceIndex) = 0;

		virtual void BindColorAttachment(uint index = 0, int slot = 0) const = 0;
		virtual uint GetColorAttachment(uint index = 0) const = 0;
		virtual uint GetDepthAttachment() const = 0;
		virtual uint GetWidth() const = 0;
		virtual uint GetHeight() const = 0;
		virtual uint GetID() const = 0;

		virtual void Blit(const TAGE::MEM::Ref<Framebuffer>& target) const = 0;
		virtual void BlitToScreen() const = 0;
		virtual void BindRead() const = 0;
		virtual void BindWrite() const = 0;

		static TAGE::MEM::Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}