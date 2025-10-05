#pragma once

#include "TARE/API/RenderAPI.h"

namespace TARE::OpenGL {
	class OpenGL_RenderAPI : public RenderAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) const override;
		virtual void Clear(uint properties = 0x0) override;
		virtual void SetViewport(int x, int y, int width, int height) override;

		virtual void DepthTest(bool enabled)  override;
		virtual void BlendTest(bool enabled)  override;

		virtual void DrawIndexed(const TAGE::MEM::Ref<VertexArrayObject>& VAO) override;
		virtual void DrawVertex(const TAGE::MEM::Ref<VertexArrayObject>& VAO)  override;
		virtual void DrawLine(const TAGE::MEM::Ref<VertexArrayObject>& VAO, uint vertexCount) override;
		virtual void DrawFullScreenQuad() override;
		virtual void RenderCube() override;
	};
}