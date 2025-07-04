#include "tagepch.h"
#include "OpenGL_Texture.h"
#include <glad/glad.h>

#define STB_DXT_IMPLEMENTATION
#include <stb/stb_dxt.h>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
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

	OpenGL_Texture2D::~OpenGL_Texture2D() { LOG_WARN("OPENGL TEXTURE DELETED"); glDeleteTextures(1, &_ID); }
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
		_Path = path.c_str();
		CPUTextureData cpuData;
		auto& grapichDistpacher = TAGE::Application::Get()->GetGraphicDispatcher();
		if (!LoadTextureCPU(path, cpuData)) {
			return LoadFallbackTexture();
		}

		grapichDistpacher.Enqueue([=]() {
			UploadToGPU(cpuData);
			});

		return true;
	}

	bool OpenGL_Texture2D::LoadTextureFromMemory(const uint8* data, size_t size)
	{
		CPUTextureData cpuData;
		auto& grapichDistpacher = TAGE::Application::Get()->GetGraphicDispatcher();

		if (!LoadTextureCPUFromMemory(data, size, cpuData)) {
			return LoadFallbackTexture();
		}
		grapichDistpacher.Enqueue([=]() {
			UploadToGPU(cpuData);
			});
		return true;
	}

	bool OpenGL_Texture2D::LoadTextureFromMemory(const uint8* data, int width, int height, int channels)
	{
		CPUTextureData cpuData;
		cpuData.Width = width;
		cpuData.Height = height;
		cpuData.Channels = channels;
		cpuData.HasAlpha = (channels == 4);
		cpuData.RawData.assign(data, data + (width * height * channels));
		cpuData.InternalFormat = cpuData.HasAlpha ? GL_RGBA8 : GL_RGB8;
		cpuData.IsCompressed = false;
		auto& grapichDistpacher = TAGE::Application::Get()->GetGraphicDispatcher();

		grapichDistpacher.Enqueue([=]() {
			UploadToGPU(cpuData);
			});
		return true;
	}

	bool OpenGL_Texture2D::SaveToDisk(const std::string& location)
	{
		return false;
	}

	bool OpenGL_Texture2D::LoadTextureCPU(const std::string& path, CPUTextureData& data)
	{
		int width, height, channels;
		uint8* rawData = stbi_load(path.c_str(), &width, &height, &channels, 4);
		if (!rawData) {
			LOG_ERROR("Failed to load Texture: {}", path);
			return false;
		}
		data.Width = width;
		data.Height = height;
		data.Channels = channels;
		data.HasAlpha = true;
		data.RawData.assign(rawData, rawData + (width * height * 4));
		stbi_image_free(rawData);

		data.IsCompressed = false;
		data.InternalFormat = GL_RGBA8;
		return true;
	}

	void OpenGL_Texture2D::UploadToGPU(const CPUTextureData& data)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &_ID);
		int mipLevels = 1 + floor(log2(std::max(data.Width, data.Height)));
		glTextureStorage2D(_ID, mipLevels, data.InternalFormat, data.Width, data.Height);
		glTextureSubImage2D(_ID, 0, 0, 0, data.Width, data.Height, GL_RGBA, GL_UNSIGNED_BYTE, data.RawData.data());
		glGenerateTextureMipmap(_ID);
		glTextureParameteri(_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		_Width = data.Width;
		_Height = data.Height;
		_Channels = data.Channels;
	}

	bool OpenGL_Texture2D::LoadTextureCPUFromMemory(const uint8* data, size_t size, CPUTextureData& outData)
	{
		int width, height, channels;
		uint8_t* raw = stbi_load_from_memory(data, (int)size, &width, &height, &channels, 4);
		if (!raw) return false;
		outData.Width = width;
		outData.Height = height;
		outData.Channels = 4;
		outData.HasAlpha = true;
		outData.RawData.assign(raw, raw + (width * height * 4));
		stbi_image_free(raw);

		outData.IsCompressed = false;
		outData.InternalFormat = GL_RGBA8;
		return true;
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