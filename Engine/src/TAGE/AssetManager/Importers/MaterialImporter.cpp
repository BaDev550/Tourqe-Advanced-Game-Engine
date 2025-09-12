#include "tagepch.h"
#include "MaterialImporter.h"
#include "TAGE/Project/Project.h"

#include "TAGE/AssetManager/Serializers/AssetSerializer.h"

namespace TAGE {
	MEM::Ref<TARE::Material> MaterialImporter::ImportMaterial(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadMaterial(Project::GetAssetDirectory() / metadata.FilePath);
	}

	MEM::Ref<TARE::Material> MaterialImporter::LoadMaterial(const std::filesystem::path& filepath)
	{
		MEM::Ref<TARE::Material> material;
		if (material = std::dynamic_pointer_cast<TARE::Material>(MaterialSerializer::Deserialize(filepath))) {
			return material;
		}
		return nullptr;
	}
}