#pragma once

#include "TAGE/TAGE.h"

namespace TAGE::Editor {
	class MaterialEditor
	{
	public:
		MaterialEditor() = default;
		static void OpenModal(AssetHandle material);
		static void Render();
	private:
		static void SelectTexture(TextureType type);
	};
}
