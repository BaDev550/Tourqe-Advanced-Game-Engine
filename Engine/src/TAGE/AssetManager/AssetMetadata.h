#pragma once
#include <filesystem>
#include "Asset.h"

namespace TAGE {
	struct AssetMetadata
	{
		AssetHandle Handle = 0;
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;

		bool IsValid() const { return Type != AssetType::None; }
	};
}