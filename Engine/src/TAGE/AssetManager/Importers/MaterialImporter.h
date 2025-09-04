#pragma once

#include "TAGE/AssetManager/AssetImporter.h"
#include "TARE/Material/Material.h"

namespace TAGE {
	class MaterialImporter
	{
	public:
		static MEM::Ref<TARE::Material> ImportMaterial(AssetHandle handle, const AssetMetadata& metadata);
		static MEM::Ref<TARE::Material> LoadMaterial(const std::filesystem::path& filepath);
	};
}
