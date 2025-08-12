#pragma once
#include <filesystem>
#include "Asset.h"

namespace TAGE {
	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;

		operator bool() const { return Type != AssetType::None; }
	};
}