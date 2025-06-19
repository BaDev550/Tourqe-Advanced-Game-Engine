#include "tagepch.h"
#include "RenderContext.h"
#include "TARE/Common/RenderAPI.h"
#include "GLFW/glfw3.h"

#include "Platform/OpenGL/OpenGL_RenderContext.h"
#include "Platform/DirectX11/DirectX11_RenderContext.h"

namespace TARE{
	TAGE::MEM::Ref<RenderContext> RenderContext::Create(void* handle)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_RenderContext>(static_cast<GLFWwindow*>(handle));
		case RAPI::DIRECTX11: return TAGE::MEM::MakeRef<DirectX11_RenderContext>(static_cast<GLFWwindow*>(handle));
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}