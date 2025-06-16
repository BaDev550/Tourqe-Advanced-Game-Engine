#include "tagepch.h"
#include "VertexArrayBuffer.h"
#include "TARE/Common/RenderAPI.h"

namespace TARE {
	VertexArrayObject* VertexArrayObject::Create()
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}