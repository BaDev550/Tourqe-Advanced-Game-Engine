#pragma once

#include "RenderAPI.h"

namespace TARE {
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color) {
			_API->SetClearColor(color);
		}
		inline static void Clear(uint properties = COLOR) {
			_API->Clear(properties);
		}
		inline static void SetViewport(int x, int y, int width, int height) {
			_API->SetViewport(x, y, width, height);
		}
		inline static void DrawIndexed(const TAGE::MEM::Ref<VertexArrayObject>& VAO) {
			_API->DrawIndexed(VAO);
		}
		inline static void DrawLine(const TAGE::MEM::Ref<VertexArrayObject>& VAO, uint vertexCount) {
			_API->DrawLine(VAO, vertexCount);
		}
		inline static void DrawVertex(const TAGE::MEM::Ref<VertexArrayObject>& VAO) {
			_API->DrawVertex(VAO);
		}
		inline static void Enable(uint prop) {
			_API->Enable(prop);
		}
		inline static void Disable(uint prop) {
			_API->Disable(prop);
		}
		inline static void BindTextureFromID(uint64 textureID, uint slot) {
			_API->BindTextureFromID(textureID, slot);
		}
		inline static void DrawFullScreenQuad() {
			_API->DrawFullScreenQuad();
		}
		inline static void SetCullingMode(CullMode mode) {
			_API->SetCullingMode(mode);
		}
		inline static void ToggleStencilFunc(StencilMode mode) {
			_API->ToggleStencilFunction(mode);
		}

	private:
		static RenderAPI* _API;
	};
}
