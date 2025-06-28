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
		_pointDepthCubemap = CubemapTexture::Create();
		_pointDepthCubemap->LoadShadowPointCubemap();

		_depthShader = ShaderLibrary::Add("ShadowDepth", "../Engine/shaders/Shadow/shadow_vertex", "../Engine/shaders/Shadow/shadow_fragment");
		_pointLightDepthShader = ShaderLibrary::Add("PointLightShadowDepth", 
			"../Engine/shaders/Shadow/point_light_shadow_vertex", 
			"../Engine/shaders/Shadow/point_light_shadow_fragment",
			"../Engine/shaders/Shadow/point_light_shadow_geometry"
		);
	}

	void ShadowMap::BeginRender(const glm::mat4& lightViewProj)
	{
		_depthFBO->Bind();
		RenderCommand::Clear(DEPTH);
		_depthShader->Use();
		_depthShader->SetUniform("u_LightSpaceMatrix", lightViewProj);

		RenderCommand::SetCullingMode(CullMode::FRONT);
	}

	void ShadowMap::EndRender()
	{
		_depthFBO->Unbind();
		RenderCommand::SetCullingMode(CullMode::BACK);
	}

	void ShadowMap::BeginPointLightShadowPass(const glm::vec3& lightPos)
	{
		_depthFBO->Bind();
		RenderCommand::SetViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
		RenderCommand::Clear(DEPTH);

		_pointLightDepthShader->Use();
		_pointLightDepthShader->SetUniform("u_LightPos", lightPos);
		_pointLightDepthShader->SetUniform("u_FarPlane", 100.0f);
		RenderCommand::SetCullingMode(CullMode::FRONT);
	}

	void ShadowMap::SetFaceViewProjection(int faceIndex, const glm::mat4& vpMatrix)
	{
		_pointDepthCubemap->AttachCubemapFaceToFBO(faceIndex, _depthFBO->GetID());
		_pointLightDepthShader->Use();
		_pointLightDepthShader->SetUniform(("shadowMatrices[" + std::to_string(faceIndex) + "]").c_str(), vpMatrix);
	}

	void ShadowMap::EndPointLightShadowPass()
	{
		_depthFBO->Unbind();
		RenderCommand::SetCullingMode(CullMode::BACK);
	}

	void ShadowMap::BindTexture(uint slot) const
	{
		RenderCommand::BindTextureFromID(_depthFBO->GetDepthAttachment(), slot);
	}
}