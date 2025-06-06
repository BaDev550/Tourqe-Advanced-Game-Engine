#include "tagepch.h"
#include "Texture.h"
#include "stb/stb_image.h"

namespace TAGE::Renderer {
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

	void Texture2D::LoadTexture(const std::string& filePath)
	{
		std::cout << filePath << std::endl;
		if (filePath.empty()) {
			LOG_WARN("Texture file path is empty, using fallback texture.");
			CreateCheckerboardTexture();
			return;
		}

		int width, height, channels;
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		if (data) {
			_Size = glm::vec2(width, height);
			_Channels = channels;
			GLenum format = (_Channels == 1) ? GL_RED : (_Channels > 3) ? GL_RGBA : GL_RGB;
			GLenum internalFormat = (_Channels == 4) ? GL_RGBA8 : GL_RGB8;

			glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
			glTextureStorage2D(_ID, 1, internalFormat, width, height);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureSubImage2D(_ID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			LOG_ERROR("Failed to load texture: {}", filePath);
			CreateCheckerboardTexture();
		}
	}

	void Texture2D::LoadFromTextureAtlas(const std::string& filePath, int x, int y, int tileWidth, int tileHeight)
	{
		int atlasWidth, atlasHeight, channels;
		unsigned char* data = stbi_load(filePath.c_str(), &atlasWidth, &atlasHeight, &channels, 0);
		if (!data) {
			LOG_ERROR("Failed to load texture atlas: {0}", filePath);
			return;
		}

		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

		int tileSize = tileWidth * tileHeight * channels;
		unsigned char* tileData = new unsigned char[tileSize];

		for (int row = 0; row < tileHeight; ++row) {
			int srcIndex = ((y + row) * atlasWidth + x) * channels;
			int dstIndex = row * tileWidth * channels;
			memcpy(&tileData[dstIndex], &data[srcIndex], tileWidth * channels);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
		glTextureStorage2D(_ID, 1, (format == GL_RGBA ? GL_RGBA8 : GL_RGB8), tileWidth, tileHeight);
		glTextureSubImage2D(_ID, 0, 0, 0, tileWidth, tileHeight, format, GL_UNSIGNED_BYTE, tileData);

		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		_Size = glm::vec2(tileWidth, tileHeight);

		delete[] tileData;
		stbi_image_free(data);
	}
	void Texture2D::LoadFromMemory(const unsigned char* data, size_t size)
	{
		int width, height, channels;
		unsigned char* pixels = stbi_load_from_memory(data, static_cast<int>(size), &width, &height, &channels, 4);
		if (!pixels)
			throw std::runtime_error("Failed to load embedded texture image");

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
	}
	void Texture2D::LoadFromMemory(const unsigned char* data, int width, int height, int channels)
	{
		if (width <= 0 || height <= 0 || channels <= 0) {
			LOG_ERROR("Invalid texture dimensions or channels: width={}, height={}, channels={}", width, height, channels);
			CreateCheckerboardTexture();
			return;
		}
		_Size = glm::vec2(static_cast<float>(width), static_cast<float>(height));
		GLenum format = GL_RGBA;
		if (channels == 1) format = GL_RED;
		else if (channels == 3) format = GL_RGB;
		else if (channels == 4) format = GL_RGBA;

		glGenTextures(1, &_ID);
		glBindTexture(GL_TEXTURE_2D, _ID);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}