#pragma once
#include "Texture.h"

namespace TARE {
	class TextureCube : public Texture
	{
	public:
		static TAGE::MEM::Ref<TextureCube> Create(const TextureSpecs& specification, TAGE::Buffer Data = TAGE::Buffer());

		virtual TextureType GetTextureType() const override { return TextureType::TextureCube; }

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::EnviromentMap; }
		virtual TAGE::AssetType GetType() const override { return GetStaticType(); }
	};
}