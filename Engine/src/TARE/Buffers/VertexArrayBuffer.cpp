#include "tagepch.h"
#include "VertexArrayBuffer.h"
#include "TARE/Common/RenderAPI.h"

#include "Platform/OpenGL/OpenGL_VertexArrayBuffer.h"

namespace TARE {
	TAGE::MEM::Ref<VertexArrayObject> VertexArrayObject::Create()
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_VertexArrayObject>();
		case RAPI::DIRECTX11:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}