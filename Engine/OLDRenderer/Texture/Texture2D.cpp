#include "tagepch.h"
#include "Texture.h"
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace TAGE::Renderer {
	static GLenum GetInternalFormat(int channels) {
		switch (channels) {
		case 1: return GL_RED;
		case 3: return GL_RGB;
		case 4: return GL_RGBA;
		default: throw std::invalid_argument("Unsupported channel count");
		}
	}

	void Texture2D::CreateCheckerboardTexture()
	{
#ifdef USE_FALLBACK_TEXTURE
		if (!g_FallbackTextureCreated) {
			_Size.x = _Size.y = FALLBACK_TEXTURE_SIZE;
			_Channels = 3;

			std::vector<unsigned char> data(_Size.x * _Size.y * _Channels);

			for (int y = 0; y < _Size.y; ++y) {
				for (int x = 0; x < _Size.x; ++x) {
					int checkerX = x / FALLBACK_TEXTURE_CHECKER_SIZE;
					int checkerY = y / FALLBACK_TEXTURE_CHECKER_SIZE;
					bool isWhite = (checkerX + checkerY) % 2 == 0;

					int index = (y * _Size.x + x) * _Channels;
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
			glTextureStorage2D(_ID, 1, GL_RGB8, _Size.x, _Size.y);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureSubImage2D(_ID, 0, 0, 0, _Size.x, _Size.y, GL_RGB, GL_UNSIGNED_BYTE, data.data());
			g_FallbackTextureCreated = true;
			g_FallbackTextureID = _ID;
		}
		else {
			LOG_WARN("Fallback texture already created, skipping creation.");
			_ID = g_FallbackTextureID;
		}
#endif
	}

    Texture2D::Texture2D(const std::string& filePath)
	{
		_Size = glm::vec2(0.0f);
		_FilePath = filePath;
		LoadTexture(filePath);
    }
	Texture2D::Texture2D(uint id, int width, int height)
		: _ID(id), _Size(glm::vec2(width, height))
	{
	}

    Texture2D::Texture2D(const std::string& filePath, glm::vec2 coords, glm::vec2 tile)
    {
		_Size = glm::vec2(0.0f);
		_FilePath = filePath;
		LoadFromTextureAtlas(filePath, static_cast<int>(coords.x), static_cast<int>(coords.y), static_cast<int>(tile.x), static_cast<int>(tile.y));
    }
	Texture2D::Texture2D(const unsigned char* data, size_t size)
	{
		_Size = glm::vec2(0.0f);
		_FilePath = "Embedded Texture";
		LoadFromMemory(data, size);
	}
	Texture2D::Texture2D(const unsigned char* data, int width, int height, int channels)
	{
		_Size = glm::vec2(0.0f);
		_FilePath = "Embedded Texture";
		LoadFromMemory(data, width, height, channels);
	}
    Texture2D::~Texture2D()
    {
		if (_ID) {
			glDeleteTextures(1, &_ID);
			_ID = 0;
		}
		_Size = glm::vec2(0.0f);
    }
    void Texture2D::Bind(uint slot) const
    {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, _ID);
    }
    void Texture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

	bool Texture2D::LoadTexture(const std::string& filePath)
	{
		if (filePath.empty()) {
			LOG_WARN("Texture file path is empty, using fallback texture.");
			CreateCheckerboardTexture();
			return true;
		}
		_FilePath = filePath;

		int width, height, channels;
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		if (data) {
			_Size = glm::vec2(width, height);
			_Channels = channels;
			GLenum internalFormat = (_Channels == 4) ? GL_RGBA8 : GL_RGB8;

			glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
			glTextureStorage2D(_ID, 1, internalFormat, width, height);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureSubImage2D(_ID, 0, 0, 0, width, height, GetInternalFormat(_Channels), GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);

			_FilePath.clear();
			_FilePath.shrink_to_fit();
		}
		else {
			LOG_ERROR("Failed to load texture: {}", filePath);
			CreateCheckerboardTexture();
		}
		return true;
	}

	void Texture2D::LoadFromTextureAtlas(const std::string& filePath, int x, int y, int tileWidth, int tileHeight)
	{
		int atlasWidth, atlasHeight, channels;
		unsigned char* data = stbi_load(filePath.c_str(), &atlasWidth, &atlasHeight, &channels, 0);
		if (!data) {
			LOG_ERROR("Failed to load texture atlas: {0}", filePath);
			return;
		}

		int tileSize = tileWidth * tileHeight * channels;
		unsigned char* tileData = new unsigned char[tileSize];

		for (int row = 0; row < tileHeight; ++row) {
			int srcIndex = ((y + row) * atlasWidth + x) * channels;
			int dstIndex = row * tileWidth * channels;
			memcpy(&tileData[dstIndex], &data[srcIndex], tileWidth * channels);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
		glTextureStorage2D(_ID, 1, (GetInternalFormat(_Channels) == GL_RGBA ? GL_RGBA8 : GL_RGB8), tileWidth, tileHeight);
		glTextureSubImage2D(_ID, 0, 0, 0, tileWidth, tileHeight, GetInternalFormat(_Channels), GL_UNSIGNED_BYTE, tileData);

		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		_Size = glm::vec2(tileWidth, tileHeight);

		delete[] tileData;
		stbi_image_free(data);
	}
	bool Texture2D::LoadFromMemory(const unsigned char* data, size_t size)
	{
		int width, height, channels;
		unsigned char* pixels = stbi_load_from_memory(data, static_cast<int>(size), &width, &height, &channels, 4);
		if (!pixels) {
			throw std::runtime_error("Failed to load embedded texture image");
			return false;
		}
		_Size = { static_cast<float>(width), static_cast<float>(height) };
		_Channels = 4;

		if (_ID != 0) {
			glDeleteTextures(1, &_ID);
			_ID = 0;
		}
		glGenTextures(1, &_ID);
		glBindTexture(GL_TEXTURE_2D, _ID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(pixels);

		_Size.x = width;
		_Size.y = height;
		return true;
	}
	bool Texture2D::LoadFromMemory(const unsigned char* data, int width, int height, int channels)
	{
		if (width <= 0 || height <= 0 || channels <= 0) {
			LOG_ERROR("Invalid texture dimensions or channels: width={}, height={}, channels={}", width, height, channels);
			CreateCheckerboardTexture();
			return true;
		}
		_Size = glm::vec2(static_cast<float>(width), static_cast<float>(height));
		_Channels = channels;

		if (_ID != 0) {
			glDeleteTextures(1, &_ID);
			_ID = 0;
		}
		glGenTextures(1, &_ID);
		glBindTexture(GL_TEXTURE_2D, _ID);

		glTexImage2D(GL_TEXTURE_2D, 0, GetInternalFormat(_Channels), width, height, 0, GetInternalFormat(_Channels), GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}

	bool Texture2D::SaveToDisk(const std::string& filepath)
	{
		_FilePath = filepath;

		if (_LocalBuffer.empty()) {
			if (_ID == 0 || _Size.x == 0 || _Size.y == 0) {
				return false;
			}

			_LocalBuffer.resize(static_cast<size_t>(_Size.x) * _Size.y * _Channels);
			glBindTexture(GL_TEXTURE_2D, _ID);
			glGetTexImage(GL_TEXTURE_2D, 0, GetInternalFormat(_Channels), GL_UNSIGNED_BYTE, _LocalBuffer.data());
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		std::filesystem::path p(filepath);
		std::string extension = p.extension().string();
		int stride_in_bytes = _Size.x * _Channels;
		int result = 0;

		if (extension == ".png") {
			result = stbi_write_png(filepath.c_str(), _Size.x, _Size.y, _Channels, _LocalBuffer.data(), stride_in_bytes);
		}
		else if (extension == ".jpg" || extension == ".jpeg") {
			result = stbi_write_jpg(filepath.c_str(), _Size.x, _Size.y, _Channels, _LocalBuffer.data(), 95);
		}
		else if (extension == ".bmp") {
			result = stbi_write_bmp(filepath.c_str(), _Size.x, _Size.y, _Channels, _LocalBuffer.data());
		}
		else {
			std::string png_path = filepath + ".png";
			result = stbi_write_png(png_path.c_str(), _Size.x, _Size.y, _Channels, _LocalBuffer.data(), stride_in_bytes);
		}

		_LocalBuffer.clear();
		_LocalBuffer.shrink_to_fit();
		return result != 0;
	}
}