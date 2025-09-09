#pragma once

#include "TAGE/TAGE.h"

namespace TAGE::GUI {
	class MaterialEditor
	{
	public:
		MaterialEditor() = default;
		static void OpenModal(AssetHandle material);
		static void Render();
	};
}
