#pragma once
#include "AssetMetadata.h"

namespace TAGE {
	class AssetImporter
	{
	public:
		static MEM::Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
	};
}