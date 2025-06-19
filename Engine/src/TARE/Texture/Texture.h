#pragma once
#include "TAGE/Common/TEnums.h"
#include <string>

namespace TARE {
	class Texture {
	public:
		virtual ~Texture() = default;
		virtual void Bind(uint8 slot = 0) const = 0;
		virtual void Unbind() const = 0;

		virtual uint GetID() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
	};

	class Texture2D : public Texture {
	public:
		virtual ~Texture2D() = default;
		virtual void Bind(uint8 slot = 0) const = 0;
		virtual void Unbind() const = 0;

		virtual std::string GetPath() const = 0;
		virtual uint GetID() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
	public:
		virtual bool LoadTexture(const std::string& path) = 0;
		virtual bool LoadTextureFromMemory(const uint8* data, size_t size) = 0;
		virtual bool LoadTextureFromMemory(const uint8* data, int width, int height, int channels) = 0;
		virtual bool LoadTextureFromAtlas(const std::string& path, int x, int y, int tileWidth, int tileHeight) = 0;
		virtual bool LoadFallbackTexture() = 0;
		virtual bool SaveToDisk(const std::string& location) = 0;

		static TAGE::MEM::Ref<Texture2D> Create();
	};
}