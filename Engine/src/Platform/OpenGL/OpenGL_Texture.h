#pragma once
#include "TARE/Texture/Texture.h"

namespace TARE {
	class OpenGL_Texture2D : public Texture2D
	{
	public:
		OpenGL_Texture2D() {}
		~OpenGL_Texture2D();
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
		virtual bool LoadFallbackTexture() override;
		virtual bool SaveToDisk(const std::string& location) override;
	private:
		uint _ID;
		int _Channels;
		int _Width, _Height;
		const char* _Path;

		bool LoadTextureCPU(const std::string& path, CPUTextureData& data);
		bool LoadTextureCPUFromMemory(const uint8* data, size_t size, CPUTextureData& outData);
		void UploadToGPU(const CPUTextureData& data);
	};

	class OpenGL_CubemapTexture : public CubemapTexture
	{
	public:
		OpenGL_CubemapTexture() {}
		virtual void Bind(uint8 slot = 0) const override;
		virtual void Unbind() const override;
		virtual bool LoadCubemap(const std::string& path) override;
		virtual bool ConvertEquirectangularToCubemap(Bitmap in, std::vector<Bitmap>& map) override;
		virtual bool LoadCubemapData(std::vector<Bitmap>& bitmaps) override;
		virtual uint LoadShadowPointCubemap() override;
		virtual void AttachCubemapFaceToFBO(int face, uint FBO) override;

		virtual uint GetID() const override { return _ID; }
		virtual int GetWidth() const override { return _Width; }
		virtual int GetHeight() const override { return _Height; }
		virtual std::string GetPath() const override { return _Path; };
	private:
		uint _ID;
		int _Channels;
		int _Width, _Height;
		std::string _Path;
	};
}