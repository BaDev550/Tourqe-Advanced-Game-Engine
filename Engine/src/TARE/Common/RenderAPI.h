#pragma once
#include "TAGE/Common/TEnums.h"

namespace TARE {
	class RenderAPI {
	public:
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear(uint properties = COLOR) = 0;
		virtual void SetViewport(int x, int y, int width, int height) = 0;
		virtual void DrawIndexed() = 0;
		virtual void DrawVertex() = 0;

		static void SetShadingMode(ShadingMode mode) { s_Mode = mode; }
		static void SetRenderMode(DebugRenderMode mode) { s_DebugRenderMode = mode; }

		static inline RAPI GetRenderAPI() { return s_API; }
		static inline ShadingMode GetShadingMode() { return s_Mode; }
		static inline DebugRenderMode GetDebugRenderMode() { return s_DebugRenderMode; }
	private:
		static RAPI s_API;
		static ShadingMode s_Mode;
		static DebugRenderMode s_DebugRenderMode;
	};
}
