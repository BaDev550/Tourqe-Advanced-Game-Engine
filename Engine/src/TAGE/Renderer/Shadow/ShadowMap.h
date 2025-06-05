#pragma once
#include "TAGE/Common/TDefines.h"

namespace TAGE::Renderer {
	class ShadowMap
	{
	public:
		ShadowMap(uint width, uint height);

		void BindDepthTexture(int slot);
		void Bind() const;
		void Unbind() const;
		uint GetDepthMap() const { return _DepthMap; }
	private:
		uint _Framebuffer;
		uint _DepthMap;
		uint _Width, _Height;
	};
}