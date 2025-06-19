#include "tagepch.h"
#include "ShadowMap.h"
#include "TARE/Common/RenderCommands.h"

namespace TARE {
	ShadowMap::ShadowMap(int width, int height)
	{
		FramebufferSpecification spec({
				FramebufferTextureFormat::DEPTH32F
			}, 1, width, height);
		_depthFBO = Framebuffer::Create(spec);

		_depthShader = ShaderLibrary::Add("ShadowDepth", "../Engine/shaders/Shadow/shadow_vertex", "../Engine/shaders/Shadow/shadow_fragment");

		SetLightDirection(glm::vec3(-2.0f, -4.0f, -1.0f));
		SetOrthoBounds(-20, 20, -20, 20, 1.0f, 100.0f);
		UpdateMatrices();
	}

	void ShadowMap::BeginRender(const glm::mat4& lightViewProj)
	{
		_depthFBO->Bind();
		RenderCommand::Clear(DEPTH);
		_depthShader->Use();
		_depthShader->SetUniform("u_LightSpaceMatrix", lightViewProj);
	}
	void ShadowMap::EndRender()
	{
		_depthFBO->Unbind();
	}
	void ShadowMap::BindTexture(uint slot) const
	{
		RenderCommand::BindTextureFromID(_depthFBO->GetDepthAttachment(), slot);
	}
	void ShadowMap::SetLightDirection(const glm::vec3& dir)
	{
		_lightDir = glm::normalize(dir);
		UpdateMatrices();
	}
	void ShadowMap::SetOrthoBounds(float left, float right, float bottom, float top, float, float)
	{
		_lightProj = glm::ortho(left, right, bottom, top, NEAR_CLIP, FAR_CLIP);
		UpdateMatrices();
	}
	void ShadowMap::UpdateMatrices()
	{
		glm::vec3 lightPos = -_lightDir * 30.0f;
		_lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
		_lightSpaceMatrix = _lightProj * _lightView;
	}
}