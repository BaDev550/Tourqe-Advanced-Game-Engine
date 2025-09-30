#include "tagepch.h"
#include "Buffers.h"

#include "Platform/OpenGL/OpenGL_Buffers.h"

namespace TARE {
	TAGE::MEM::Ref<VertexBufferObject> VertexBufferObject::Create(void* data, size_t size)
	{
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: return TAGE::MEM::MakeRef<OpenGL::OpenGL_VertexBufferObject>(data, size);
		default:
			break;
		}
	}
	TAGE::MEM::Ref<ElementBufferObject> ElementBufferObject::Create(uint* indices, size_t size)
	{
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: return TAGE::MEM::MakeRef<OpenGL::OpenGL_ElementBufferObject>(indices, size);
		default:
			break;
		}
	}
}