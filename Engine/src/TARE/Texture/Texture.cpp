#include "tagepch.h"
#include "Texture.h"
#include "TARE/Common/RenderAPI.h"

#include "Platform/OpenGL/OpenGL_Texture.h"

namespace TARE {
	TAGE::MEM::Ref<Texture2D> TARE::Texture2D::Create(const TextureSpecs& specs, TAGE::Buffer data)
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_Texture2D>(specs, data);
		case RAPI::DIRECTX11:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}

	TAGE::MEM::Ref<CubemapTexture> CubemapTexture::Create()
	{
		switch (RenderAPI::GetRenderAPI())
		{
		case RAPI::UNDEFINED:
		case RAPI::OPENGL: return TAGE::MEM::MakeRef<OpenGL_CubemapTexture>();
		case RAPI::DIRECTX11:
		case RAPI::VULKAN:
		default:
			ASSERT_NOMSG(false);
			break;
		}
		return nullptr;
	}
}