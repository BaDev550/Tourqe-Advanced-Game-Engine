#pragma once
#include "AssetManagerBase.h"
#include "TAGE/Project/Project.h"

namespace TAGE {
	class AssetManager
	{
	public:
		template<typename T>
		static MEM::Ref<T> GetAsset(AssetHandle handle) {
			return std::static_pointer_cast<T>(Project::GetActive()->GetAssetManager()->GetAsset(handle));
		}
	};
}