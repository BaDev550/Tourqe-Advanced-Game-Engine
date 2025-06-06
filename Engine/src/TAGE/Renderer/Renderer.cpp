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

	void GL_Renderer::BeginFrame(const MEM::Ref<Camera>& cam, const std::vector<Light>& lights)
	{
		_SceneData.CameraPosition = cam->GetPosition();
		_SceneData.CameraDirection = cam->GetForward();
		_SceneData.CameraUp = cam->GetUp();
		_SceneData.ViewMatrix = cam->GetViewMatrix();
		_SceneData.ProjectionMatrix = cam->GetProjectionMatrix();
		_SceneData.ViewProjectionMatrix = cam->GetViewProjectionMatrix();
		
		_PostProcessPass->Begin();

		_SceneData.Lights.clear();
		_SceneData.Lights = lights;

		_MainShader->Use();
		_MainShader->SetUniform("u_ViewProjection", _SceneData.ViewProjectionMatrix);
		_MainShader->SetUniform("u_CameraPosition", _SceneData.CameraPosition);
		_ShadowPass->BindShadowMap();
		_MainShader->SetUniform("u_ShadowMap", SHADOW_MAP_TEXTURE_SLOT);
		_MainShader->SetUniform("u_LightSpaceMatrix", _SceneData.LightViewProjectionMatrix);
		_MainShader->SetUniform("u_LightCount", (int)_SceneData.Lights.size());

		int lightIndex = 0;
		for (const auto& light : _SceneData.Lights)
		{
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].type").c_str(), static_cast<int>(light.type));
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].position").c_str(),  light.position);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].direction").c_str(), light.direction);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].color").c_str(),     light.color);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].intensity").c_str(), light.intensity);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].range").c_str(),     light.range);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].innerCone").c_str(), light.innerCone);
			_MainShader->SetUniform(("u_Lights[" + std::to_string(lightIndex) + "].outerCone").c_str(), light.outerCone);
			if (light.type == LightType::DIRECTIONAL){
				_MainShader->SetUniform("u_ShadowLightIndex", lightIndex);
				_SceneData.ShadowLightIndex = lightIndex;
			}
#if DEBUG_RENDERER_LIGHTS
			_LightDebuggerRenderer->Render(cam, light);
#endif
			lightIndex++;
		}

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

	void GL_Renderer::EndFrame()
	{
		_PostProcessPass->End();

		//_BloomPass->ExtractBloom(_PostProcessPass->GetColorAttachment());
		//_BloomPass->End();
		_PostProcessPass->Render();
	}

	std::vector<glm::vec3> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
	{
		glm::mat4 inv = glm::inverse(proj * view);
		std::vector<glm::vec3> corners;
		for (unsigned int x = 0; x < 2; ++x)
		{
			for (unsigned int y = 0; y < 2; ++y)
			{
				for (unsigned int z = 0; z < 2; ++z)
				{
					glm::vec4 pt = inv * glm::vec4(
						2.0f * x - 1.0f,
						2.0f * y - 1.0f,
						2.0f * z - 1.0f,
						1.0f);
					corners.push_back(glm::vec3(pt) / pt.w);
				}
			}
		}
		return corners;
	}

	void GL_Renderer::CalculateDirectLightVPMatrix()
	{
		Light& light = _SceneData.Lights.at(_SceneData.ShadowLightIndex);
		if (light.type == LightType::DIRECTIONAL) {
			//auto corners = GetFrustumCornersWorldSpace(_SceneData.ProjectionMatrix, _SceneData.ViewMatrix);

			//glm::vec3 min = glm::vec3(FLT_MAX);
			//glm::vec3 max = glm::vec3(-FLT_MAX);

			//for (const auto& corner : corners)
			//{
			//	min = glm::min(min, corner);
			//	max = glm::max(max, corner);
			//}

			//glm::vec3 frustumCenter = (min + max) * 0.5f;
			//float radius = glm::length(max - min) * 0.5f;
			//glm::vec3 lightPos = frustumCenter - light.direction * radius * 2.0f;
			//float shadowMapResolution = 1024.0f;
			//float worldUnitsPerTexel = (radius * 2.0f) / shadowMapResolution;
			//lightPos.x = floor(lightPos.x / worldUnitsPerTexel) * worldUnitsPerTexel;
			//lightPos.y = floor(lightPos.y / worldUnitsPerTexel) * worldUnitsPerTexel;
			//lightPos.z = floor(lightPos.z / worldUnitsPerTexel) * worldUnitsPerTexel;

			//glm::mat4 lightView = glm::lookAt(lightPos, frustumCenter, glm::vec3(0, 1, 0));

			//glm::vec3 minLS = glm::vec3(FLT_MAX);
			//glm::vec3 maxLS = glm::vec3(-FLT_MAX);

			//for (const auto& corner : corners)
			//{
			//	glm::vec3 cornerLS = glm::vec3(lightView * glm::vec4(corner, 1.0f));
			//	minLS = glm::min(minLS, cornerLS);
			//	maxLS = glm::max(maxLS, cornerLS);
			//}

			//glm::mat4 lightProjection = glm::ortho(minLS.x, maxLS.x, minLS.y, maxLS.y, -maxLS.z - 10.0f, -minLS.z + 10.0f);
			//glm::mat4 lightViewProj = lightProjection * lightView;
			//_SceneData.LightViewProjectionMatrix = lightViewProj;

			glm::mat4 lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
			glm::mat4 lightView = glm::lookAt(
				light.direction,
				glm::vec3(0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
			_SceneData.LightViewProjectionMatrix = lightProjection * lightView;
		}
	}
}