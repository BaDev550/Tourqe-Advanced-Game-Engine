#include "tagepch.h"
#include "RenderCommands.h"

#include "Platform/OpenGL/OpenGL_RenderAPI.h"

namespace TARE {
	RenderAPI* RenderCommand::_API = new OpenGL_RenderAPI;
}