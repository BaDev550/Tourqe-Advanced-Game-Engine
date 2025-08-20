#include "tagepch.h"
#include "AssetImporter.h"

#include "Importers/TextureImporter.h"
#include "Importers/SceneImporter.h"

namespace TAGE {
	using AssetImportFunction = std::function<MEM::Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{ AssetType::Texture, TextureImporter::ImportTexture2D },
		{ AssetType::Scene,   SceneImporter::ImportScene },
	};

	MEM::Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end()) {
			LOG_WARN("No importer for asset type: {}", (int)metadata.Type);
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}
}