#include "tagepch.h"
#include "Renderer.h"
#include "Shader/ShaderLibrary.h"
#include "TAGE/Application/Application.h"

namespace TAGE::Renderer {
	GL_Renderer::GL_Renderer(int width, int height)
	{
		_MainShader = ShaderLibrary::Add("MainShader", "../Engine/shaders/main_vertex.glsl", "../Engine/shaders/main_fragment.glsl");
		_GridShader = ShaderLibrary::Add("GridShader", "../Engine/shaders/grid_vertex.glsl", "../Engine/shaders/grid_fragment.glsl");
		_SkyboxShader = ShaderLibrary::Add("SkyBoxShader", "../Engine/shaders/skybox_vertex.glsl", "../Engine/shaders/skybox_fragment.glsl");
		_PostProcessPass = MEM::MakeScope<PostProcessPass>(width, height);
		_BloomPass = MEM::MakeScope<BloomPass>(width, height);
		_ShadowPass = MEM::MakeScope<ShadowPass>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
		_SkyBox = MEM::MakeScope<Skybox>();

		_Deferred = MEM::MakeScope<DeferredRendering>(width, height);
#if DEBUG_RENDERER_LIGHTS
		_LightDebuggerRenderer = MEM::MakeScope<Debugger::Debugger_Light>();
		_LightDebuggerRenderer->Init();
#endif
	}

	void GL_Renderer::Shutdown()
	{

	}

	void GL_Renderer::BeginFrame(const MEM::Ref<Camera>& cam)
	{
		_MainShader->Use();
		_MainShader->SetUniform("u_ViewProjection", _SceneData.ViewProjectionMatrix);
		_MainShader->SetUniform("u_CameraPosition", _SceneData.CameraPosition);
		_MainShader->SetUniform("u_LightSpaceMatrix", _SceneData.LightViewProjectionMatrix);

		_ShadowPass->BindShadowMap();
		_MainShader->SetUniform("u_ShadowMap", SHADOW_MAP_TEXTURE_SLOT);
	}

	void GL_Renderer::BeginGBuffer(const MEM::Ref<Camera>& cam)
	{
		_SceneData.CameraPosition = cam->GetPosition();
		_SceneData.CameraDirection = cam->GetForward();
		_SceneData.CameraUp = cam->GetUp();
		_SceneData.ViewMatrix = cam->GetViewMatrix();
		_SceneData.ProjectionMatrix = cam->GetProjectionMatrix();
		_SceneData.ViewProjectionMatrix = cam->GetViewProjectionMatrix();

		_Deferred->BeginGeometryPass();
		auto shader = _Deferred->GetShader();

		shader->Use();
		shader->SetUniform("u_View", cam->GetViewMatrix());
		shader->SetUniform("u_Projection", cam->GetProjectionMatrix());
		_Deferred->RenderGeometryPass();

		_SkyBox->DrawSkybox(_SceneData.ViewMatrix, _SceneData.ProjectionMatrix);

		if (DEBUG_RENDERER_USE_GRID && _SceneData.DrawGrid)
			_EndlessGrid.Render(cam);
	}

	void GL_Renderer::EndGBuffer()
	{
		for (const auto& light : _SceneData.Lights)
			_Deferred->RenderLightPass(light.position, light.color, _SceneData.CameraPosition, _SceneData.LightViewProjectionMatrix);

		_PostProcessPass->Begin();
		_PostProcessPass->SetInputTexture(_Deferred->GetTexture(DeferredRendering::LIGHT)->GetID());
	}

	void GL_Renderer::BeginShadowMap()
	{
		_ShadowPass->Begin();
		_ShadowPass->SetLightProjectionMatrix(_SceneData.LightViewProjectionMatrix);
		Clear(DEPTH);
	}

	void GL_Renderer::EndShadowMap()
	{
		_ShadowPass->End();
	}

	void GL_Renderer::EndFrame()
	{
		_PostProcessPass->End();
		_PostProcessPass->Render();
	}

	void GL_Renderer::CalculateLight(Light& light, int index)
	{
		std::string baseUniform = "u_Lights[" + std::to_string(index) + "]";
		_MainShader->SetUniform((baseUniform + ".type").c_str(), static_cast<int>(light.type));
		_MainShader->SetUniform((baseUniform + ".position").c_str(), light.position);
		_MainShader->SetUniform((baseUniform + ".direction").c_str(), light.direction);
		_MainShader->SetUniform((baseUniform + ".color").c_str(), light.color);
		_MainShader->SetUniform((baseUniform + ".intensity").c_str(), light.intensity);
		_MainShader->SetUniform((baseUniform + ".range").c_str(), light.range);
		_MainShader->SetUniform((baseUniform + ".innerCone").c_str(), light.innerCone);
		_MainShader->SetUniform((baseUniform + ".outerCone").c_str(), light.outerCone);

		if (light.type == LightType::DIRECTIONAL) {
			float shadowRange = 10.0f;
			glm::mat4 lightProjection = glm::ortho(-shadowRange, shadowRange, -shadowRange, shadowRange, 1.0f, 50.0f);
			glm::mat4 lightView = glm::lookAt(
				light.direction,
				glm::vec3(0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);

			_SceneData.LightViewProjectionMatrix = lightProjection * lightView;
		}
#if DEBUG_RENDERER_LIGHTS
			_LightDebuggerRenderer->Render(cam, light);
#endif
	}

	void GL_Renderer::SetLights(std::vector<Light>& lights)
	{
		_SceneData.Lights = lights;
		_MainShader->SetUniform("u_LightCount", static_cast<int>(lights.size()));
	}
}