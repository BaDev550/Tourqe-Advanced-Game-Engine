#include "tagepch.h"
#include "Renderer.h"
#include "Shader/ShaderLibrary.h"
#include "GLFW/glfw3.h"
#include "TAGE/Application/Application.h"

namespace TAGE::Renderer {
	GL_Renderer::GL_Renderer(int width, int height)
	{
		_MainShader = ShaderLibrary::Add("MainShader", "../Engine/shaders/main_vertex.glsl", "../Engine/shaders/main_fragment.glsl");
		_GridShader = ShaderLibrary::Add("GridShader", "../Engine/shaders/grid_vertex.glsl", "../Engine/shaders/grid_fragment.glsl");
		_SkyboxShader = ShaderLibrary::Add("SkyBoxShader", "../Engine/shaders/skybox_vertex.glsl", "../Engine/shaders/skybox_fragment.glsl");
		_PostProcessPass = MEM::MakeScope<PostProcessPass>(width, height);
		_BloomPass = MEM::MakeScope<BloomPass>(width, height);
		_ShadowPass = MEM::MakeScope<ShadowPass>(1028, 1028);
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
		_SceneData.CameraPosition = cam->GetPosition();
		_SceneData.CameraDirection = cam->GetForward();
		_SceneData.CameraUp = cam->GetUp();
		_SceneData.ViewMatrix = cam->GetViewMatrix();
		_SceneData.ProjectionMatrix = cam->GetProjectionMatrix();
		_SceneData.ViewProjectionMatrix = cam->GetViewProjectionMatrix();
		
		_PostProcessPass->Begin();

		_MainShader->Use();
		_MainShader->SetUniform("u_ViewProjection", _SceneData.ViewProjectionMatrix);
		_MainShader->SetUniform("u_CameraPosition", _SceneData.CameraPosition);
		_MainShader->SetUniform("u_LightSpaceMatrix", _SceneData.LightViewProjectionMatrix);
		_MainShader->SetUniform("u_LightCount", (int)_SceneData.Lights.size());

		_ShadowPass->BindShadowMap();
		_MainShader->SetUniform("u_ShadowMap", SHADOW_MAP_TEXTURE_SLOT);

		_SkyBox->DrawSkybox(_SceneData.ViewMatrix, _SceneData.ProjectionMatrix);

		if(DEBUG_RENDERER_USE_GRID && _SceneData.DrawGrid)
			_EndlessGrid.Render(cam);
	}

	void GL_Renderer::BeginGBuffer(const MEM::Ref<Camera>& cam)
	{
		_Deferred->BeginGeometryPass();
		_Deferred->GetShader()->Use();
		_Deferred->GetShader()->SetUniform("u_View", cam->GetViewMatrix());
		_Deferred->GetShader()->SetUniform("u_Projection", cam->GetProjectionMatrix());
		_Deferred->RenderGeometryPass();
	}

	void GL_Renderer::EndGBuffer()
	{
		for (const auto& light : _SceneData.Lights)
			_Deferred->RenderLightPass(light.position, light.color, _SceneData.CameraPosition);
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

	void GL_Renderer::CalculateLights(const std::vector<Light>& lights)
	{
		_SceneData.Lights = lights;

		int lightIndex = 0;
		for (const auto& light : _SceneData.Lights)
		{
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].type").c_str(), static_cast<int>(light.type));
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].position").c_str(), light.position);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].direction").c_str(), light.direction);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].color").c_str(), light.color);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].intensity").c_str(), light.intensity);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].range").c_str(), light.range);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].innerCone").c_str(), light.innerCone);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].outerCone").c_str(), light.outerCone);
			if (light.type == LightType::DIRECTIONAL) {
				glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
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
			lightIndex++;
		}
	}

}