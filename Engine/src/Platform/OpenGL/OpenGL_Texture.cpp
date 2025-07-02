#include "tagepch.h"
#include "OpenGL_Texture.h"
#include <glad/glad.h>

#define STB_DXT_IMPLEMENTATION
#include <stb/stb_dxt.h>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

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

	OpenGL_Texture2D::~OpenGL_Texture2D()
	{
		glDeleteTextures(1, &_ID);
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

		int width, height, channels;
		uint8* raw_data = stbi_load(_Path, &width, &height, &channels, 4);
		if (!raw_data) {
			LOG_ERROR("Failed To Load Texture: {}", _Path);
			LoadFallbackTexture();
			return false;
		}

		_Width = width;
		_Height = height;
		_Channels = 4;

		bool hasAlpha = (channels == 4);
		GLenum internalFormat = hasAlpha ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

		int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) ? 16 : 8;
		int compressedSize = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;

		auto* compressed_data = new uint8[compressedSize];
		uint8* dest_ptr = compressed_data;

		for (int y = 0; y < height; y += 4)
		{
			for (int x = 0; x < width; x += 4)
			{
				uint8 source_block[16 * 4];
				for (int block_y = 0; block_y < 4; ++block_y)
				{
					for (int block_x = 0; block_x < 4; ++block_x)
					{
						int src_x = x + block_x;
						int src_y = y + block_y;

						uint8* src_pixel_ptr;
						if (src_x < width && src_y < height) {
							src_pixel_ptr = raw_data + (src_y * width + src_x) * 4;
						}
						else {
							int clamped_x = std::min(width - 1, src_x);
							int clamped_y = std::min(height - 1, src_y);
							src_pixel_ptr = raw_data + (clamped_y * width + clamped_x) * 4;
						}
						uint8* dest_pixel_ptr = source_block + (block_y * 4 + block_x) * 4;
						memcpy(dest_pixel_ptr, src_pixel_ptr, 4);
					}
				}
				stb_compress_dxt_block(dest_ptr, source_block, hasAlpha, STB_DXT_HIGHQUAL);
				dest_ptr += blockSize;
			}
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
		int mipLevels = 1 + floor(log2(std::max(width, height)));
		glTextureStorage2D(_ID, mipLevels, internalFormat, width, height);
		glCompressedTextureSubImage2D(_ID, 0, 0, 0, width, height, internalFormat, compressedSize, compressed_data);
		glGenerateTextureMipmap(_ID);

		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(raw_data);
		delete[] compressed_data;

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