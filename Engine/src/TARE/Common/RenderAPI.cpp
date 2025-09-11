#include "tagepch.h"
#include "RenderAPI.h"

namespace TARE {
	RAPI RenderAPI::s_API = RAPI::DIRECTX11;
	ShadingMode RenderAPI::s_Mode = ShadingMode::SHADED;
	DebugRenderMode RenderAPI::s_DebugRenderMode = DebugRenderMode::NONE;
}