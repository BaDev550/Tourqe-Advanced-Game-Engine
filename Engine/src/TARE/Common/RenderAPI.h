#pragma once
#include "TAGE/Common/TEnums.h"
#include "TARE/Buffers/VertexArrayBuffer.h"

namespace TARE {
	class RenderAPI {
	public:
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear(uint properties = COLOR) = 0;
		virtual void SetViewport(int x, int y, int width, int height) = 0;
		virtual void DrawIndexed(const TAGE::MEM::Ref<VertexArrayObject>& VAO) = 0;
		virtual void DrawVertex(const TAGE::MEM::Ref<VertexArrayObject>& VAO) = 0;
		virtual void DrawLine(const TAGE::MEM::Ref<VertexArrayObject>& VAO, uint vertexCount) = 0;
		virtual void Enable(uint properties) = 0;
		virtual void Disable(uint properties) = 0;
		virtual void BindTextureFromID(uint64 textureID, uint slot) = 0;
		virtual void DrawFullScreenQuad() = 0;
		virtual void SetCullingMode(CullMode mode) = 0;
		virtual void ToggleStencilFunction(StencilMode mode) = 0;

		static void SetShadingMode(ShadingMode mode) { s_Mode = mode; }
		static void SetRenderMode(DebugRenderMode mode) { s_DebugRenderMode = mode; }

		static inline RAPI GetRenderAPI() { return s_API; }
		static inline ShadingMode GetShadingMode() { return s_Mode; }
		static inline DebugRenderMode GetDebugRenderMode() { return s_DebugRenderMode; }
	private:
		static RAPI s_API;
		static ShadingMode s_Mode;
		static DebugRenderMode s_DebugRenderMode;
	};
}
