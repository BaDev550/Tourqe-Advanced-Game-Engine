#pragma once
#include "TAGE/Utilities/Memory.h"

namespace TARE {
	class RenderContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void* GetDevice() { return nullptr; };

		static TAGE::MEM::Ref<RenderContext> Create(void* handle);
	};
}
