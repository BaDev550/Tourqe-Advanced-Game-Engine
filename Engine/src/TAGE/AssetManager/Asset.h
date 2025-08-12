#pragma once
#include "TAGE/Utilities/UUID.h"

namespace TAGE {
	enum class AssetType {
		None = 0,
		Scene,
		Mesh,
		Texture,
	};

	using AssetHandle = UUID;
	class Asset
	{
	public:
		AssetHandle _handle;

		virtual AssetType GetType() const = 0;
	};
}