#pragma once
#include "TARE/Buffers/Framebuffer.h"

namespace TARE {
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
		virtual void Clear(uint attachment, int value) override;
		virtual const FramebufferSpecification& GetSpecification() const override { return _Specification; }

		virtual uint GetColorAttachment(uint index = 0) const override { ASSERT(index < _ColorAttachments.size(), "Index out of bounds for color attachments!"); return _ColorAttachments[index]; }
		virtual uint GetDepthAttachment() const override { ASSERT(_DepthAttachment != 0, "No depth attachment found!"); return _DepthAttachment; }
		virtual uint GetWidth() const override { return _Specification.Width; }
		virtual uint GetHeight() const override { return _Specification.Height; }

		virtual void Blit(const TAGE::MEM::Ref<Framebuffer>& target) const override;
		virtual void BindRead() const override;
		virtual void BindWrite() const override;
	private:
		uint _BufferID;

		FramebufferSpecification _Specification;

		std::vector<FramebufferTextureSpecification> _ColorAttachmentSpecifications;
		FramebufferTextureSpecification _DepthAttachmentSpecification = FramebufferTextureFormat::NONE;

		std::vector<uint> _ColorAttachments;
		uint _DepthAttachment = 0;
	};
}

