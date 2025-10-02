#include "tagepch.h"
#include "Framebuffer.h"

#include "Platform/OpenGL/OpenGL_Framebuffer.h"

namespace TARE {
	TAGE::MEM::Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: return TAGE::MEM::MakeRef<OpenGL::OpenGL_Framebuffer>(spec);
		default:
			break;
		}
	}
}