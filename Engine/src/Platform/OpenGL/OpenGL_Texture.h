#pragma once
#include "TARE/Texture/Texture.h"

namespace TARE {
	class OpenGL_Texture2D : public Texture2D
	{
	public:
		OpenGL_Texture2D() {}
		virtual void Bind(uint8 slot = 0) const override;
		virtual void Unbind() const override;

		virtual std::string GetPath() const { return _Path; }
		virtual uint GetID() const override { return _ID; }
		virtual int GetWidth() const override { return _Width; }
		virtual int GetHeight() const override { return _Height; }
	public:
		virtual bool LoadTexture(const std::string& path) override;
		virtual bool LoadTextureFromMemory(const uint8* data, size_t size) override;
		virtual bool LoadTextureFromMemory(const uint8* data, int width, int height, int channels) override;
		virtual bool LoadTextureFromAtlas(const std::string& path, int x, int y, int tileWidth, int tileHeight) override;
		virtual bool LoadFallbackTexture() override;
		virtual bool SaveToDisk(const std::string& location) override;
	private:
		uint _ID;
		int _Channels;
		int _Width, _Height;
		const char* _Path;
	};
}