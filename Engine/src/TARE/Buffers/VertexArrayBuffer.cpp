#include "tagepch.h"
#include "VertexArrayBuffer.h"

#include "Platform/OpenGL/OpenGL_VertexArrayBuffer.h"

namespace TARE {
	TAGE::MEM::Ref<VertexArrayObject> VertexArrayObject::Create() {
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: return TAGE::MEM::MakeRef<OpenGL::OpenGL_VertexArrayObject>();
		default:
			return nullptr;
		}
	}
}