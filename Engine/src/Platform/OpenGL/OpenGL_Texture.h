#pragma once
#include "TARE/Texture/Texture.h"

namespace TARE {
	class OpenGL_Texture2D : public Texture2D
	{
	public:
		OpenGL_Texture2D(const TextureSpecs& specs, TAGE::Buffer data = TAGE::Buffer());
		~OpenGL_Texture2D();
		virtual void Bind(uint8 slot = 0) const override;
		virtual void Unbind() const override;

		virtual uint GetID() const override { return _ID; }
		virtual int GetWidth() const override { return _Specification.Width; }
		virtual int GetHeight() const override { return _Specification.Height; }
	public:
		virtual bool LoadTexture(TAGE::Buffer& data) override;
		virtual bool LoadTextureFromMemory(const uint8* data, size_t size) override;
		virtual bool LoadTextureFromMemory(const uint8* data, int width, int height, int channels) override;
	private:
		uint _ID;
		TextureSpecs _Specification;
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