#pragma once
#include "TARE/Buffers/Framebuffer.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TARE/Camera/Camera.h"

namespace TARE {
	class DeferredRendering
	{
	public:
		DeferredRendering(int width, int height);
		~DeferredRendering();

		void RenderGeometryPass(const TAGE::MEM::Ref<Camera>& camera) const;
		void RenderLightingPass(std::vector<Light>& lights, const glm::vec3& cameraPos) const;
		void BindGBuffer() const { _GBuffer->Bind(); _GBufferShader->Use(); }
		void BindLightingBuffer() const { _LightingBuffer->Bind(); _LightShader->Use(); }
		void UnbindGBuffer() const { _GBuffer->Unbind(); }
		void UnbindLightingBuffer() const { _LightingBuffer->Unbind(); }
		void BlitToScreen() const;

		TAGE::MEM::Ref<Framebuffer> GetGBuffer() const { return _GBuffer; }
		TAGE::MEM::Ref<Framebuffer> GetLightingBuffer() const { return _LightingBuffer; }
		TAGE::MEM::Ref<Shader> GetGBufferShader() const { return _GBufferShader; }
		TAGE::MEM::Ref<Shader> GetLightShader() const { return _LightShader; }
	private:
		TAGE::MEM::Ref<Framebuffer> _GBuffer;
		TAGE::MEM::Ref<Framebuffer> _LightingBuffer;
		TAGE::MEM::Ref<Shader> _FinalShader;
		TAGE::MEM::Ref<Shader> _GBufferShader;
		TAGE::MEM::Ref<Shader> _LightShader;

		void CreateGBuffer(int width, int height);
		void CreateLightingBuffer();
	};
}