#pragma once
#include "RenderAPI.h"

namespace TARE {
	class RenderCommand
	{
	public:
		static void Create();
		static void Destroy();

		inline static void SetClearColor(const glm::vec4& color) {
			_API->SetClearColor(color);
		}
		inline static void Clear(uint properties = 0x0) {
			_API->Clear(properties);
		}
		inline static void SetViewport(int x, int y, int width, int height) {
			_API->SetViewport(x, y, width, height);
		}

		inline static void DepthTest(bool enabled) {
			_API->DepthTest(enabled);
		}
		inline static void BlendTest(bool enabled) {
			_API->BlendTest(enabled);
		}

		inline static void DrawIndexed(const TAGE::MEM::Ref<VertexArrayObject>& VAO) {
			_API->DrawIndexed(VAO);
		}
		inline static void DrawVertex(const TAGE::MEM::Ref<VertexArrayObject>& VAO) {
			_API->DrawVertex(VAO);
		}
		inline static void DrawLine(const TAGE::MEM::Ref<VertexArrayObject>& VAO, uint vertexCount) {
			_API->DrawLine(VAO, vertexCount);
		}
		inline static void DrawFullScreenQuad() {
			_API->DrawFullScreenQuad();
		}
	private:
		static inline RenderAPI* _API = nullptr;
	};
}