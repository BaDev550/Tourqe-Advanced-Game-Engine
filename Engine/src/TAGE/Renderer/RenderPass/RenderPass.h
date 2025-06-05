#pragma once

namespace TAGE::Renderer {
	class Framebuffer;
	class RenderPass {
	public:
		virtual ~RenderPass() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Render() = 0;
		virtual Framebuffer* GetBuffer() = 0;
	};
}