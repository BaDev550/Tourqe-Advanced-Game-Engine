#pragma once
#include "TAGE/Common/TDefines.h"

#include <cstdint>
#include "GLAD/glad.h"
#include "glm/glm.hpp"

namespace TAGE::Renderer {
	class Texture2D
	{
	public:
		Texture2D(const std::string& filePath);
		Texture2D(uint id, int width, int height);
		Texture2D(const std::string& filePath, glm::vec2 coords, glm::vec2 tile);
		Texture2D(const unsigned char* data, size_t size);
		Texture2D(const unsigned char* data, int width, int height, int channels);
		~Texture2D();
		void Bind(uint slot = 0) const;
		void Unbind() const;

		uint GetID() const { return _ID; }
		glm::vec2 GetSize() const { return _Size; }
	private:
		uint _ID;
		std::string _FilePath;
		glm::vec2 _Size;
		int _Channels;

		void CreateCheckerboardTexture();
		void LoadTexture(const std::string& filePath);
		void LoadFromTextureAtlas(const std::string& filePath, int x, int y, int tileWidth, int tileHeight);
		void LoadFromMemory(const unsigned char* data, size_t size);
		void LoadFromMemory(const unsigned char* data, int width, int height, int channels);
	};
}