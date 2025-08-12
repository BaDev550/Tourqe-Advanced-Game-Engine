#pragma once
#include "TAGE/AssetManager/AssetMetadata.h"
#include "TARE/Texture/Texture.h"

namespace TAGE {
	class TextureImporter
	{
	public:
		static MEM::Ref<TARE::Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
	};
}