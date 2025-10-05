#pragma once

#include "TARE/Texture/TextureCube.h"

namespace TARE::OpenGL {
	class OpenGL_TextureCube : public TextureCube
	{
	public:
		OpenGL_TextureCube(const TextureSpecs& specification, TAGE::Buffer Data = TAGE::Buffer());
		virtual void Bind(uint8 slot = 0) const override;
		virtual void Unbind() const override;
		virtual void GenerateMipmap() const override;

		virtual uint GetID() const override { return _ID; }
		virtual int GetWidth() const override { return _Specification.Width; }
		virtual int GetHeight() const override { return _Specification.Height; }
	private:
		TextureSpecs _Specification;
		uint _ID;
	};
}