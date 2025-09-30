#pragma once

#include "TARE/Texture/Texture2D.h"

namespace TARE::OpenGL {
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
}