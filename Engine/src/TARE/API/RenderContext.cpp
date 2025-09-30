#include "tagepch.h"
#include "RenderContext.h"
#include "GLFW/glfw3.h"

#include "Platform/OpenGL/OpenGL_RenderContext.h"

namespace TARE {
	TAGE::MEM::Ref<RenderContext> RenderContext::Create(void* handle)
	{
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: return TAGE::MEM::MakeRef<OpenGL::OpenGL_RenderContext>(static_cast<GLFWwindow*>(handle));
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}