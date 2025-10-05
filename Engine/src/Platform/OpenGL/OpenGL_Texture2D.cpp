#include "tagepch.h"
#include "OpenGL_Texture2D.h"
#include "OpenGL_TextureUtils.h"

#include "TAGE/Utilities/Buffer.h"
#include <stb/stb_image.h>

#include <GLAD/glad.h>

namespace TARE::OpenGL {
	OpenGL_Texture2D::~OpenGL_Texture2D() {
		if (_ID != 0) {
			glDeleteTextures(1, &_ID);
		}
	}
	OpenGL_Texture2D::OpenGL_Texture2D(const TextureSpecs& specs, TAGE::Buffer& data) : _ID(0), _Specification(specs)
	{
		if (_Specification.format == TextureFormat::NONE) {
			_Specification.format = GetDataFormat(_Specification.Channels);
		}
		if (_Specification.InternalFormat == TextureFormat::NONE) {
			_Specification.InternalFormat = GetInternalFormat(_Specification.Channels);
		}
		LoadTexture(data);
	}

	void OpenGL_Texture2D::Bind(uint8 slot) const
	{
		if (_ID == 0) {
			return;
		}
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, _ID);
	}
	void OpenGL_Texture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGL_Texture2D::GenerateMipmap() const
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	bool OpenGL_Texture2D::LoadTexture(TAGE::Buffer& data)
	{
		int mipLevels = 1 + static_cast<int>(floor(log2(std::max(_Specification.Width, _Specification.Height))));
		if (_ID != 0)
			glDeleteTextures(1, &_ID);
		
		GLenum InternalFormat = TextureFormatToOpenGLInternalFormat(_Specification.InternalFormat);
		GLenum baseFormat = TextureFormatToOpenGLBaseFormat(_Specification.format);
		GLenum type = TextureDataTypeToOpenGLType(_Specification.DataType);
		
		glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
		glTextureStorage2D(_ID, mipLevels, InternalFormat, _Specification.Width, _Specification.Height);
		glTextureSubImage2D(_ID, 0, 0, 0, _Specification.Width, _Specification.Height, baseFormat, type, data.As<float>());
		if (_Specification.CreateMipmaps)
			glGenerateTextureMipmap(_ID);
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		data.Release();

		return true;
	}

	bool OpenGL_Texture2D::LoadTextureFromMemory(const uint8* data, size_t size)
	{
		return true;
	}

	bool OpenGL_Texture2D::LoadTextureFromMemory(const uint8* data, int width, int height, int channels)
	{
		return true;
	}
}