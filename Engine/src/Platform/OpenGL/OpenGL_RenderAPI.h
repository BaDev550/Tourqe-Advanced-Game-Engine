#pragma once
#include "TARE/Common/RenderAPI.h"

namespace TARE {
	class OpenGL_RenderAPI : public RenderAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear(uint properties = COLOR) override;
		virtual void SetViewport(int x, int y, int width, int height) override;
		virtual void DrawIndexed() override;
		virtual void DrawVertex() override;
	};
}