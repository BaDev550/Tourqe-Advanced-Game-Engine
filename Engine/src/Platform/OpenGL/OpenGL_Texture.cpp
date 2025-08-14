#include "tagepch.h"
#include "OpenGL_Texture.h"
#include <glad/glad.h>
#include "TAGE/Application/Application.h"

namespace TARE {
	namespace Utils {
		static GLenum GetDataFormat(int channels) {
			switch (channels) {
			case 1: return GL_RED;
			case 3: return GL_RGB;
			case 4: return GL_RGBA;
			default:
				ASSERT(false, "Unsupported channel count");
				return 0;
			}
		}

		static GLenum GetInternalFormat(int channels) {
			switch (channels)
			{
			case 1: return GL_RED;
			case 3: return GL_RGB8;
			case 4: return GL_RGBA8;
			default:
				ASSERT(false, "Unsupported channel count");
			}
		}
	}

	OpenGL_Texture2D::~OpenGL_Texture2D() { 
		if (_ID != 0) {
			glDeleteTextures(1, &_ID);
		}
	}
	OpenGL_Texture2D::OpenGL_Texture2D(const TextureSpecs& specs, TAGE::Buffer data) : _ID(0), _Specification(specs)
	{
		_Specification.format = Utils::GetDataFormat(_Specification.Channels);
		_Specification.InternalFormat = Utils::GetInternalFormat(_Specification.Channels);
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

	bool OpenGL_Texture2D::LoadTexture(TAGE::Buffer& data)
	{
		int mipLevels = 1 + static_cast<int>(floor(log2(std::max(_Specification.Width, _Specification.Height))));
		if (_ID != 0)
			glDeleteTextures(1, &_ID);

		glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
		glTextureStorage2D(_ID, mipLevels, _Specification.InternalFormat, _Specification.Width, _Specification.Height);
		glTextureSubImage2D(_ID, 0, 0, 0, _Specification.Width, _Specification.Height, _Specification.format, GL_UNSIGNED_BYTE, data.Data);
		glGenerateTextureMipmap(_ID);
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

#if 0
	bool OpenGL_Texture2D::LoadFallbackTexture()
	{
		LOG_WARN("Using fallback texture");
#ifdef USE_FALLBACK_TEXTURE
		if (!g_FallbackTextureCreated) {
			_Width = _Height = FALLBACK_TEXTURE_SIZE;
			_Channels = 3;

			std::vector<unsigned char> data(_Width * _Height * _Channels);

			for (int y = 0; y < _Height; ++y) {
				for (int x = 0; x < _Width; ++x) {
					int checkerX = x / FALLBACK_TEXTURE_CHECKER_SIZE;
					int checkerY = y / FALLBACK_TEXTURE_CHECKER_SIZE;
					bool isWhite = (checkerX + checkerY) % 2 == 0;

					int index = (y * _Width + x) * _Channels;
					if (isWhite) {
						data[index + 0] = 200;
						data[index + 1] = 200;
						data[index + 2] = 200;
					}
					else {
						data[index + 0] = 50;
						data[index + 1] = 50;
						data[index + 2] = 50;
					}
				}
			}

			glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
			glTextureStorage2D(_ID, 1, GL_RGB8, _Width, _Height);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureSubImage2D(_ID, 0, 0, 0, _Width, _Height, GL_RGB, GL_UNSIGNED_BYTE, data.data());
			g_FallbackTextureCreated = true;
			g_FallbackTextureID = _ID;
		}
		else {
			LOG_WARN("Fallback texture already created, skipping creation.");
			_ID = g_FallbackTextureID;
		}
		return true;
#endif
	}
#endif

}