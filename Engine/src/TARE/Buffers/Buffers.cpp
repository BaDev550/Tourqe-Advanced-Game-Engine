#include "tagepch.h"
#include "Buffers.h"
#include "TARE/Common/RenderAPI.h"

#include "Platform/OpenGL/OpenGL_Buffers.h"

namespace TARE {
	VertexBufferObject* VertexBufferObject::Create(float* vertices, uint16 size)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return new OpenGL_VertexBufferObject(vertices, size);
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}

	ElementBufferObject* ElementBufferObject::Create(uint* indices, uint size)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return new OpenGL_ElementBufferObject(indices, size);
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}