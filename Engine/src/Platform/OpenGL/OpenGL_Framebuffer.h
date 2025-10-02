#pragma once

#include "TARE/Buffers/Framebuffer.h"
#include "TAGE/Utilities/Memory.h"

namespace TARE::OpenGL {
	class OpenGL_Framebuffer : public Framebuffer
	{
	public:
		OpenGL_Framebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGL_Framebuffer();
		void Invalidate();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void Resize(uint width, uint height) override;
		virtual int Read(uint attachment, int x, int y) const override;
		virtual void ClearAttachment(uint attachment, int value) override;
		virtual const FramebufferSpecification& GetSpecification() const override { return _Specification; }

		virtual void BindColorAttachment(uint index = 0, int slot = 0) const override;
		virtual uint GetColorAttachment(uint index = 0) const override { CHECKF(index < _ColorAttachments.size(), "Index out of bounds for color attachments!"); return _ColorAttachments[index]; }
		virtual uint GetDepthAttachment() const override { CHECKF(_DepthAttachment != 0, "No depth attachment found!"); return _DepthAttachment; }
		virtual uint GetWidth() const override { return _Specification.Width; }
		virtual uint GetHeight() const override { return _Specification.Height; }
		virtual uint GetID() const override { return _BufferID; }

		virtual void Blit(const TAGE::MEM::Ref<Framebuffer>& target) const override;
		virtual void BlitToScreen() const override;
		virtual void BindRead() const override;
		virtual void BindWrite() const override;
	private:
		uint _BufferID;

		FramebufferSpecification _Specification;
		std::vector<FramebufferTextureSpecification> _ColorAttachmentSpecifications;
		std::vector<uint> _ColorAttachments;
		uint _DepthAttachment = 0;
		FramebufferTextureSpecification _DepthAttachmentSpecification = FramebufferTextureFormat::NONE;
	};
}