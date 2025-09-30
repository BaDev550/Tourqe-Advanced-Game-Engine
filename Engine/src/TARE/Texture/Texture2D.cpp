#include "tagepch.h"
#include "Texture2D.h"

#include "Platform/OpenGL/OpenGL_Texture2D.h"

namespace TARE {
	TAGE::MEM::Ref<Texture2D> Texture2D::Create(const TextureSpecs& specs, TAGE::Buffer data) {
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: return TAGE::MEM::MakeRef<OpenGL::OpenGL_Texture2D>(specs, data);
		default:
			break;
		}
	}
}