#include "tagepch.h"
#include "Buffers.h"
#include "TARE/Common/RenderAPI.h"

#include "Platform/OpenGL/OpenGL_Buffers.h"

namespace TARE {
	TAGE::MEM::Ref<VertexBufferObject> VertexBufferObject::Create(void* data, size_t size)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_VertexBufferObject>(data, size);
		case RAPI::DIRECTX11:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}

	TAGE::MEM::Ref<ElementBufferObject> ElementBufferObject::Create(uint* indices, size_t size)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_ElementBufferObject>(indices, size);
		case RAPI::DIRECTX11:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}