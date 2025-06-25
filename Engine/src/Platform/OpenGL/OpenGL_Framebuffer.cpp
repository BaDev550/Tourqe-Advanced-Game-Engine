#include "tagepch.h"
#include "OpenGL_Framebuffer.h"
#include <GLAD/glad.h>

namespace TARE {
	static const uint s_MaxFramebufferSize = 8192;
	namespace Utils {
		static GLenum TextureFormatTo_OpenGLFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
			case FramebufferTextureFormat::RGBA16F: return GL_RGBA16F;
			case FramebufferTextureFormat::RGBA: return GL_RGBA;
			case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			ASSERT_NOMSG(false);
			return 0;
		}

		static GLenum TextureFormatTo_OpenGLType(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8:         return GL_UNSIGNED_BYTE;
			case FramebufferTextureFormat::RGBA16F:       return GL_HALF_FLOAT;
			case FramebufferTextureFormat::RGBA:          return GL_FLOAT;
			case FramebufferTextureFormat::RED_INTEGER:   return GL_INT;
			}

			ASSERT_NOMSG(false);
			return 0;
		}

		static GLenum TextureTarget(bool multisampled) { return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; }
		static void CreateTextures(bool multisampled, uint* outID, uint count) { glCreateTextures(TextureTarget(multisampled), count, outID); }
		static void BindTexture(bool multisampled, uint id) { glBindTexture(TextureTarget(multisampled), id); }
		static void AttachColorTexture(uint id, int samples, FramebufferTextureFormat formatEnum, GLenum internalFormat, GLenum format, uint width, uint height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				GLenum type = TextureFormatTo_OpenGLType(formatEnum);

				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint id, int samples, GLenum format, GLenum attachmentType, uint width, uint height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			case FramebufferTextureFormat::DEPTH24:  return true;
			case FramebufferTextureFormat::DEPTH32F:  return true;
			}

			return false;
		}
	}

	OpenGL_Framebuffer::OpenGL_Framebuffer(const FramebufferSpecification& spec)
		: _Specification(spec)
	{
		for (auto spec : _Specification.Attachments.Attachments) {
			if (!Utils::IsDepthFormat(spec.format))
				_ColorAttachmentSpecifications.push_back(spec);
			else
				_DepthAttachmentSpecification = spec;
		}
		Invalidate();
	}

	OpenGL_Framebuffer::~OpenGL_Framebuffer()
	{
		glDeleteFramebuffers(1, &_BufferID);
		glDeleteTextures(_ColorAttachments.size(), _ColorAttachments.data());
		glDeleteTextures(1, &_DepthAttachment);
	}

	void OpenGL_Framebuffer::Invalidate()
	{
		if (_BufferID)
		{
			glDeleteFramebuffers(1, &_BufferID);
			glDeleteTextures(_ColorAttachments.size(), _ColorAttachments.data());
			glDeleteTextures(1, &_DepthAttachment);

			_ColorAttachments.clear();
			_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &_BufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, _BufferID);
		bool multisample = _Specification.Samples > 1;

		if (_ColorAttachmentSpecifications.size())
		{
			_ColorAttachments.resize(_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, _ColorAttachments.data(), _ColorAttachments.size());

			for (size_t i = 0; i < _ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multisample, _ColorAttachments[i]);
				switch (_ColorAttachmentSpecifications[i].format)
				{
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(_ColorAttachments[i], _Specification.Samples, FramebufferTextureFormat::RGBA8, GL_RGBA8, GL_RGBA, _Specification.Width, _Specification.Height, i);
					break;
				case FramebufferTextureFormat::RGBA:
					Utils::AttachColorTexture(_ColorAttachments[i], _Specification.Samples, FramebufferTextureFormat::RGBA, GL_RGBA, GL_RGBA, _Specification.Width, _Specification.Height, i);
					break;
				case FramebufferTextureFormat::RGBA16F:
					Utils::AttachColorTexture(_ColorAttachments[i], _Specification.Samples, FramebufferTextureFormat::RGBA16F, GL_RGBA16F, GL_RGBA, _Specification.Width, _Specification.Height, i);
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					Utils::AttachColorTexture(_ColorAttachments[i], _Specification.Samples, FramebufferTextureFormat::RED_INTEGER, GL_R32I, GL_RED_INTEGER, _Specification.Width, _Specification.Height, i);
					break;
				}
			}
		}

		if (_DepthAttachmentSpecification.format != FramebufferTextureFormat::NONE)
		{
			Utils::CreateTextures(multisample, &_DepthAttachment, 1);
			Utils::BindTexture(multisample, _DepthAttachment);
			switch (_DepthAttachmentSpecification.format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(_DepthAttachment, _Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, _Specification.Width, _Specification.Height);
				break;
			case FramebufferTextureFormat::DEPTH32F:
				Utils::AttachDepthTexture(_DepthAttachment, _Specification.Samples, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, _Specification.Width, _Specification.Height);
				break;
			case FramebufferTextureFormat::DEPTH24:
				Utils::AttachDepthTexture(_DepthAttachment, _Specification.Samples, GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT, _Specification.Width, _Specification.Height);
				break;
			}
		}

		if (_ColorAttachments.size() > 1)
		{
			ASSERT_NOMSG(_ColorAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(_ColorAttachments.size(), buffers);
		}
		else if (_ColorAttachments.empty())
		{
			glReadBuffer(GL_NONE);
			glDrawBuffer(GL_NONE);
		}

		ASSERT_NOMSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _BufferID);
		glViewport(0, 0, _Specification.Width, _Specification.Height);
	}

	void OpenGL_Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Framebuffer::Resize(uint width, uint height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			ASSERT_NOMSG(false, "Attempted to resize framebuffer to invalid size!");
			return;
		}
		_Specification.Width = width;
		_Specification.Height = height;

		Invalidate();
	}

	int OpenGL_Framebuffer::Read(uint attachment, int x, int y) const
	{
		ASSERT_NOMSG(attachment < _ColorAttachmentSpecifications.size(), "Invalid attachment index for Read");

		const auto& spec = _ColorAttachmentSpecifications[attachment];
		GLenum format = Utils::TextureFormatTo_OpenGLFormat(spec.format);
		GLenum type = Utils::TextureFormatTo_OpenGLType(spec.format);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);

		int pixelData = 0;
		glReadPixels(x, y, 1, 1, format, type, &pixelData);
		return pixelData;
	}


	void OpenGL_Framebuffer::Clear(uint attachment, int value)
	{
		auto& spec = _ColorAttachmentSpecifications[attachment];
		GLenum format = Utils::TextureFormatTo_OpenGLFormat(spec.format);
		GLenum type = Utils::TextureFormatTo_OpenGLType(spec.format);

		glClearTexImage(_ColorAttachments[attachment], 0, format, type, &value);
	}

	void OpenGL_Framebuffer::Blit(const TAGE::MEM::Ref<Framebuffer>& target) const
	{
		BindRead();
		target->BindWrite();

		glBlitFramebuffer(
			0, 0, GetWidth(), GetHeight(),
			0, 0, target->GetWidth(), target->GetHeight(),
			GL_COLOR_BUFFER_BIT, GL_NEAREST
		);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Framebuffer::BindRead() const
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _BufferID);
	}

	void OpenGL_Framebuffer::BindWrite() const
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _BufferID);
	}
}