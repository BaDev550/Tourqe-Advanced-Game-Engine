#pragma once
#include "TARE/Common/RenderAPI.h"

namespace TARE {
	class OpenGL_RenderAPI : public RenderAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear(uint properties = COLOR) override;
		virtual void SetViewport(int x, int y, int width, int height) override;
		virtual void DrawIndexed(const TAGE::MEM::Ref<VertexArrayObject>& VAO) override;
		virtual void DrawVertex(const TAGE::MEM::Ref<VertexArrayObject>& VAO) override;
		virtual void DrawLine(const TAGE::MEM::Ref<VertexArrayObject>& VAO, uint vertexCount) override;
		virtual void Enable(uint properties) override;
		virtual void Disable(uint properties) override;
		virtual void BindTextureFromID(uint64 textureID, uint slot) override;
		virtual void DrawFullScreenQuad() override;
		virtual void SetCullingMode(CullMode mode) override;
		virtual void ToggleStencilFunction(bool mode) override;
	};
}