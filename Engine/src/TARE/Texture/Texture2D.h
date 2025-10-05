#pragma once
#include "Texture.h"

namespace TARE {
	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual TextureType GetTextureType() const override { return TextureType::Texture2D; }
	public:
		virtual bool LoadTexture(TAGE::Buffer& data) = 0;
		virtual bool LoadTextureFromMemory(const uint8* data, size_t size) = 0;
		virtual bool LoadTextureFromMemory(const uint8* data, int width, int height, int channels) = 0;

		static TAGE::MEM::Ref<Texture2D> Create(const TextureSpecs& specs, TAGE::Buffer& data = TAGE::Buffer());
	};
}
