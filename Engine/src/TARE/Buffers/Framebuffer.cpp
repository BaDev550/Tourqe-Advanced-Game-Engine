#include "tagepch.h"
#include "Framebuffer.h"
#include "TARE/Common/RenderAPI.h"

#include "Platform/OpenGL/OpenGL_Framebuffer.h"

namespace TARE {
	TAGE::MEM::Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_Framebuffer>(spec);
		case RAPI::DIRECTX11:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}