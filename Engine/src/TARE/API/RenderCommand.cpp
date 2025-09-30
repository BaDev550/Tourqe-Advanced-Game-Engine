#include "tagepch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGL_RenderAPI.h"

namespace TARE {
	void RenderCommand::Create()
	{
		switch (RenderAPI::GetGrapichAPI())
		{
		case GrapichAPI::OpenGL: _API = new OpenGL::OpenGL_RenderAPI();
		default:
			break;
		}
	}

	void RenderCommand::Destroy()
	{
		delete _API;
		_API = nullptr;
	}
}