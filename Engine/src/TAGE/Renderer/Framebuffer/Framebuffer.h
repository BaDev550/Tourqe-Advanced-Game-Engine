#pragma once
#include "TAGE/Common/TDefines.h"
#include <string>

namespace TAGE::Renderer {
	class Framebuffer
	{
	public:
		Framebuffer() = default;
		Framebuffer(uint width, uint height, bool hdr = false);
		~Framebuffer();
		void Bind() const;
		void Unbind() const;
		void Resize(uint width, uint height);
		void Clear() const;
		void RenderToScreen(const std::string& Sshader = "PostProcess") const;

		uint GetColorAttachmentID() const { return _ColorAttachmentID; }
		uint GetDepthAttachmentID() const { return _DepthAttachmentID; }
		int GetWidth() const { return _Width; }
		int GetHeight() const { return _Height; }

		uint GetQuadVAO() const { return _QuadVAO; }
	private:
		void Invalidate();

		uint _FramebufferID = 0;
		uint _ColorAttachmentID = 0;
		uint _DepthAttachmentID = 0;

		bool _HDR = false;
		int _Width = 0;
		int _Height = 0;

		uint _QuadVAO, _QuadVBO;
		void InitFullScreenQuad();
	};
}