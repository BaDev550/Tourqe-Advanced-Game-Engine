#pragma once
#include "TAGE/Common/TDefines.h"

#include <cstdint>
#include "GLAD/glad.h"
#include "glm/glm.hpp"
#include <string>

namespace TAGE::Renderer {
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void Bind(uint slot = 0) const = 0;
		virtual void Unbind() const = 0;

		virtual uint GetID() const = 0;
		virtual std::string GetPath() const { return ""; }
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D() {}
		Texture2D(const std::string& filePath);
		Texture2D(uint id, int width, int height);
		Texture2D(const std::string& filePath, glm::vec2 coords, glm::vec2 tile);
		Texture2D(const unsigned char* data, size_t size);
		Texture2D(const unsigned char* data, int width, int height, int channels);
		~Texture2D();
		virtual void Bind(uint slot = 0) const override;
		virtual void Unbind() const override;

		bool LoadTexture(const std::string& filePath);
		bool LoadFromMemory(const unsigned char* data, size_t size);
		bool LoadFromMemory(const unsigned char* data, int width, int height, int channels);
		bool SaveToDisk(const std::string& filepath);

		virtual uint GetID() const override { return _ID; }
		virtual std::string GetPath() const { return _FilePath; }
		glm::vec2 GetSize() const { return _Size; }
	private:
		uint _ID;
		std::string _FilePath;
		glm::vec2 _Size;
		int _Channels;
		std::vector<unsigned char> _LocalBuffer;

		void CreateCheckerboardTexture();
		void LoadFromTextureAtlas(const std::string& filePath, int x, int y, int tileWidth, int tileHeight);
	};

	class TextureCubeMap : public Texture {
	public:
		TextureCubeMap(const std::vector<std::string>& faces);
		~TextureCubeMap();

		void Bind(uint32_t slot = 0) const override;
		void Unbind() const override;

		uint32_t GetID() const override { return _ID; }
	private:
		uint _ID;
		glm::vec2 _Size;
		int _Channels;
	};
}