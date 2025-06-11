#include "tagepch.h"
#include "Framebuffer.h"
#include "GLAD/glad.h"

#include "TAGE/Renderer/Shader/ShaderLibrary.h"

namespace TAGE::Renderer {
	Framebuffer::Framebuffer(uint width, uint height, bool hdr)
		: _Width(width), _Height(height), _HDR(hdr)
	{
		Invalidate();
		InitFullScreenQuad();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &_FramebufferID);
		glDeleteTextures(1, &_ColorAttachmentID);
		glDeleteTextures(1, &_DepthAttachmentID);
	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _FramebufferID);
		glViewport(0, 0, _Width, _Height);
	}

	void Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(uint width, uint height)
	{
		_Width = width;
		_Height = height;
		Invalidate();
	}

	void Framebuffer::Clear() const
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(COLOR_DEPTH_STENCIL);
	}

	void Framebuffer::RenderToScreen(const std::string& Sshader) const
	{
		MEM::Ref<Shader> shader = ShaderLibrary::Get(Sshader);
		shader->Use();

		glBindVertexArray(_QuadVAO);
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _ColorAttachmentID);
		shader->SetUniform("u_ScreenTexture", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}

	void Framebuffer::Invalidate()
	{
		if (_FramebufferID)
		{
			glDeleteFramebuffers(1, &_FramebufferID);
			glDeleteTextures(1, &_ColorAttachmentID);
			glDeleteRenderbuffers(1, &_DepthAttachmentID);
		}

		GLenum internalFormat = _HDR ? GL_RGBA16F : GL_RGBA8;
		GLenum format = GL_RGBA;
		GLenum type = _HDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

		glGenFramebuffers(1, &_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, _FramebufferID);

		// Color attachment
		glGenTextures(1, &_ColorAttachmentID);
		glBindTexture(GL_TEXTURE_2D, _ColorAttachmentID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _Width, _Height, 0, format, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ColorAttachmentID, 0);

		// Depth attachment
		glGenRenderbuffers(1, &_DepthAttachmentID);
		glBindRenderbuffer(GL_RENDERBUFFER, _DepthAttachmentID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _Width, _Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _DepthAttachmentID);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG_ERROR("ERROR: Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::InitFullScreenQuad()
	{
		float quadVertices[] = {
			-1.0f,  1.0f,   0.0f, 1.0f,
			-1.0f, -1.0f,   0.0f, 0.0f,
			 1.0f, -1.0f,   1.0f, 0.0f,

			-1.0f,  1.0f,   0.0f, 1.0f,
			 1.0f, -1.0f,   1.0f, 0.0f,
			 1.0f,  1.0f,   1.0f, 1.0f
		};

		glGenVertexArrays(1, &_QuadVAO);
		glGenBuffers(1, &_QuadVBO);
		glBindVertexArray(_QuadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, _QuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);
	}
}