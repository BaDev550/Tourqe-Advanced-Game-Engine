#pragma once

#include "TAGE/TAGE.h"

namespace TAGE::Editor {
	class ModelEditor
	{
	public:
		ModelEditor() = default;
		static void OpenModal(MEM::Ref<TARE::Model> model);
		static void Render();
	};
}