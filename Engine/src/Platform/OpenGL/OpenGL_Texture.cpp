#include "tagepch.h"
#include "OpenGL_Texture.h"
#include <GLAD/glad.h>
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

namespace TARE {
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

	void OpenGL_Texture2D::Bind(uint8 slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, _ID);
	}
	void OpenGL_Texture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	bool OpenGL_Texture2D::LoadTexture(const std::string& path)
	{
		if (path.empty()) {
			LoadFallbackTexture();
			return true;
		}
		_Path = path.c_str();

		uint8* data = stbi_load(_Path, &_Width, &_Height, &_Channels, 0);
		if (data) {
			glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
			glTextureStorage2D(_ID, 1, GetInternalFormat(_Channels), _Width, _Height);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTextureSubImage2D(_ID, 0, 0, 0, _Width, _Height, GetDataFormat(_Channels), GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			LOG_ERROR("Failed To Load Texture: {}", _Path);
			LoadFallbackTexture();
		}
		return true;
	}
	bool OpenGL_Texture2D::LoadTextureFromMemory(const uint8* data, size_t size)
	{
		uint8* pixels = stbi_load_from_memory(data, static_cast<int>(size), &_Width, &_Height, &_Channels, 4);
		if (pixels) {
			glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
			glTextureStorage2D(_ID, 1, GetInternalFormat(_Channels), _Width, _Height);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureSubImage2D(_ID, 0, 0, 0, _Width, _Height, GetDataFormat(_Channels), GL_UNSIGNED_BYTE, data);
			stbi_image_free(pixels);
		}
		return false;
	}
	bool OpenGL_Texture2D::LoadTextureFromMemory(const uint8* data, int width, int height, int channels)
	{
		if (width <= 0 || height <= 0 || channels <= 0) {
			LoadFallbackTexture();
			return true;
		}

		if (data) {
			glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
			glTextureStorage2D(_ID, 1, GetInternalFormat(_Channels), _Width, _Height);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureSubImage2D(_ID, 0, 0, 0, _Width, _Height, GetDataFormat(_Channels), GL_UNSIGNED_BYTE, data);
		}
		return false;
	}
	bool OpenGL_Texture2D::LoadTextureFromAtlas(const std::string& path, int x, int y, int tileWidth, int tileHeight)
	{
		unsigned char* data = stbi_load(path.c_str(), &_Width, &_Height, &_Channels, 0);
		if (!data) {
			LOG_ERROR("Failed to load texture atlas: {0}", path);
			return false;
		}

		int tileSize = tileWidth * tileHeight * _Channels;
		unsigned char* tileData = new unsigned char[tileSize];

		for (int row = 0; row < tileHeight; ++row) {
			int srcIndex = ((y + row) * _Width + x) * _Channels;
			int dstIndex = row * tileWidth * _Channels;
			memcpy(&tileData[dstIndex], &data[srcIndex], tileWidth * _Channels);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
		glTextureStorage2D(_ID, 1, GetInternalFormat(_Channels), tileWidth, tileHeight);
		glTextureSubImage2D(_ID, 0, 0, 0, tileWidth, tileHeight, GetDataFormat(_Channels), GL_UNSIGNED_BYTE, tileData);

		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		delete[] tileData;
		stbi_image_free(data);
		return true;
	}

	bool OpenGL_Texture2D::SaveToDisk(const std::string& location)
	{
		return false;
	}

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

}