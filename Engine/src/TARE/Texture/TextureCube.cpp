#include "tagepch.h"
#include "TextureCube.h"

#include "Platform/OpenGL/OpenGL_TextureCube.h"

namespace TARE {
	TAGE::MEM::Ref<TextureCube> TextureCube::Create(const TextureSpecs& specification, TAGE::Buffer Data) {
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: return TAGE::MEM::MakeRef<OpenGL::OpenGL_TextureCube>(specification, Data);
		default:
			break;
		}
	}
}