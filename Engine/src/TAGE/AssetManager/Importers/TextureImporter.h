#pragma once
#include "TAGE/AssetManager/AssetMetadata.h"

#include "TARE/Texture/Texture2D.h"
#include "TARE/Texture/TextureCube.h"
#include "TARE/Types/EnviromentMap.h"

namespace TAGE {
	class TextureImporter
	{
	public:
		static MEM::Ref<TARE::Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
		static MEM::Ref<TARE::Texture2D> LoadTexture2D(const std::filesystem::path& filepath);
		static bool WriteTextureTo(const std::filesystem::path& filepath, const std::filesystem::path& to = "");
	};

	class EnviromentMapImporter
	{
	public:
		static MEM::Ref<TARE::EnviromentMap> ImportEnviromentMap(AssetHandle handle, const AssetMetadata& metadata);
		static MEM::Ref<TARE::EnviromentMap> LoadEnviromentMap(const std::filesystem::path& filepath);
	};
}