#pragma once
#include "TAGE/Common/TDefines.h"
#include "TARE/Buffers/VertexArrayBuffer.h"

namespace TARE {
	enum class GrapichAPI {
		OpenGL,
		DirectX11
	};

	class RenderAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) const = 0;
		virtual void Clear(uint properties = 0x0) = 0;
		virtual void SetViewport(int x, int y, int width, int height) = 0;

		virtual void DepthTest(bool enabled) = 0;
		virtual void BlendTest(bool enabled) = 0;

		virtual void DrawIndexed(const TAGE::MEM::Ref<VertexArrayObject>& VAO) = 0;
		virtual void DrawVertex(const TAGE::MEM::Ref<VertexArrayObject>& VAO) = 0;
		virtual void DrawLine(const TAGE::MEM::Ref<VertexArrayObject>& VAO, uint vertexCount) = 0;
		virtual void DrawFullScreenQuad() = 0;
		virtual void RenderCube() = 0;

		static inline GrapichAPI GetGrapichAPI() { return s_API; }
	private:
		static GrapichAPI s_API;
	};
}
