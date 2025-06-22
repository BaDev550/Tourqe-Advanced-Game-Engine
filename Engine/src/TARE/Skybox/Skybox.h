#pragma once
#include "TARE/Texture/Texture.h"
#include "TARE/Buffers/VertexArrayBuffer.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TAGE/Utilities/Memory.h"

namespace TARE {
	class Skybox
	{
	public:
		Skybox(const char* path);
		~Skybox();
		void Bind(const glm::mat4& view, const glm::mat4& projection) const;

		void SetTexture(const char* path);
		TAGE::MEM::Ref<CubemapTexture> GetTexture() const { return _SkyboxTexture; }
	private:
		TAGE::MEM::Ref<Shader> _SkyboxShader;
		TAGE::MEM::Ref<CubemapTexture> _SkyboxTexture;
		TAGE::MEM::Ref<VertexArrayObject> _SkyboxVAO;
	};
}
