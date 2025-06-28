#pragma once
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Memory.h"

namespace TARE {
	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Resize(uint width, uint height) = 0;
		virtual int Read(uint attachment, int x, int y) const = 0;
		virtual void Clear(uint attachment, int value) = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;

		virtual uint GetColorAttachment(uint index = 0) const = 0;
		virtual uint GetDepthAttachment() const = 0;
		virtual uint GetWidth() const = 0;
		virtual uint GetID() const = 0;
		virtual uint GetHeight() const = 0;

		virtual void Blit(const TAGE::MEM::Ref<Framebuffer>& target) const = 0;
		virtual void BindRead() const = 0;
		virtual void BindWrite() const = 0;

		static TAGE::MEM::Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};
}