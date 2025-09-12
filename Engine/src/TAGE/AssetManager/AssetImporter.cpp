#include "tagepch.h"
#include "AssetImporter.h"

#include "Importers/TextureImporter.h"
#include "Importers/SceneImporter.h"
#include "Importers/MeshImporter.h"
#include "Importers/MaterialImporter.h"

#include "Serializers/AssetSerializer.h"

namespace TAGE {
	using AssetImportFunction = std::function<MEM::Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	using AssetSerializeFunction = std::function<bool(const MEM::Ref<Asset>&, const std::filesystem::path&)>;
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{ AssetType::Texture, TextureImporter::ImportTexture2D },
		{ AssetType::Scene,     SceneImporter::ImportScene },
		{ AssetType::MeshSource, MeshImporter::ImportSourceModel },
		{ AssetType::StaticMesh, MeshImporter::ImportStaticMesh },
		{ AssetType::Material,   MaterialImporter::ImportMaterial },
	};
	static std::map<AssetType, AssetSerializeFunction> s_AssetSerializerFunctions = {
		{ AssetType::StaticMesh, StaticMeshSerializer::Serialize },
		{ AssetType::Material,   MaterialSerializer::Serialize }
	};

	MEM::Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end()) {
			LOG_WARN("No importer for asset type: {}", (int)metadata.Type);
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}

	void AssetImporter::SaveAsset(MEM::Ref<Asset>& asset, const AssetMetadata& metadata)
	{
		if (s_AssetSerializerFunctions.find(metadata.Type) == s_AssetSerializerFunctions.end()) {
			LOG_WARN("No serializer for asset type: {}", (int)metadata.Type);
			return;
		}

		s_AssetSerializerFunctions.at(metadata.Type)(asset, Project::GetAssetDirectory() / metadata.FilePath);
	}
}