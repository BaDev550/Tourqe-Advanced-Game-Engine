#pragma once

#include "TAGE/TAGE.h"

namespace TAGE {
	class MaterialEditor
	{
	public:
		MaterialEditor() = default;
		static void OpenModal(AssetHandle material);
		static void Render();
	};
}
