#pragma once

#include "TAGE/Common/TDefines.h"
#include "TAGE/Renderer/Texture/Texture.h"
#include "TAGE/Renderer/Shader/Shader.h"

namespace TAGE::Renderer {
	class DeferredRendering
	{
	public:
		enum BufferTextureType : uint {
			POS,
			NORMAL,
			ALBEDO,
			DEPTH,
			LIGHT
		};

		DeferredRendering(int width, int height);

		Texture2D* GetTexture(BufferTextureType type) {
			switch (type)
			{
			case DeferredRendering::POS: return    _gPositionTex.get();
			case DeferredRendering::NORMAL: return _gNormalTex.get();
			case DeferredRendering::ALBEDO: return _gAlbedoTex.get();
			case DeferredRendering::DEPTH: return _gDepthTex.get();
			case DeferredRendering::LIGHT: return _gLightPassTex.get();
			}
		}

		void BeginGeometryPass();
		void RenderGeometryPass();
		void RenderLightPass(const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& cameraPos, const glm::mat4& lightMatrix);
		MEM::Ref<Shader>& GetShader() { return _DeferredShader; }
	private:
		uint gBuffer;
		uint _LightPassFBO = 0;
		int _Width, _Height;

		MEM::Ref<Shader> _DeferredShader;
		MEM::Ref<Shader> _LightDeferredShader;
		MEM::Scope<Texture2D> _gLightPassTex;
		MEM::Scope<Texture2D> _gPositionTex;
		MEM::Scope<Texture2D> _gNormalTex;
		MEM::Scope<Texture2D> _gAlbedoTex;
		MEM::Scope<Texture2D> _gDepthTex;
	};
}