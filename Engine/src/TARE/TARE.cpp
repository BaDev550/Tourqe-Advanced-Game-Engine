#include "tagepch.h"
#include "TARE.h"

namespace TARE {
	TARE::TARE(int width, int height)
	{
		ShaderLibrary::Add("MainShader", "../Engine/shaders/main_vertex", "../Engine/shaders/main_fragment");
		_SingleColor = ShaderLibrary::Add("SingleColor", "../Engine/shaders/Debugger/single_color_vertex", "../Engine/shaders/Debugger/single_color_fragment");
		_Width = width;
		_Height = height;

		_Grid = TAGE::MEM::MakeScope<EndlessGrid>();
		_DeferredRendering = TAGE::MEM::MakeRef<DeferredRendering>(width, height);
		_ShadowMap = TAGE::MEM::MakeRef<ShadowMap>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
	}

	void TARE::BeginFrame(const TAGE::MEM::Ref<Camera>& cam)
	{
		_DeferredRendering->RenderGeometryPass(cam);

		_Data.PrevViewProjMatrix = _Data.ViewProjectionMatrix;
		_Data.CameraPosition = cam->GetPosition();
		_Data.CameraDirection = cam->GetForward();
		_Data.CameraUp = cam->GetUp();
		_Data.ViewMatrix = cam->GetViewMatrix();
		_Data.ProjectionMatrix = cam->GetProjectionMatrix();
		_Data.ViewProjectionMatrix = cam->GetViewProjectionMatrix();

		_DeferredRendering->GetLightShader()->Use();
		_ShadowMap->BindTexture(SHADOW_MAP_TEXTURE_SLOT);
		_DeferredRendering->GetLightShader()->SetUniform("u_ShadowMap", SHADOW_MAP_TEXTURE_SLOT);
		_DeferredRendering->GetLightShader()->SetUniform("u_LightSpaceMatrix", _Data.LightViewProjectionMatrix);
		_DeferredRendering->GetLightShader()->SetUniform("u_CameraPos", _Data.CameraPosition);

		_DeferredRendering->GetLightShader()->SetUniform("u_PrevViewProj", _Data.PrevViewProjMatrix);
		_DeferredRendering->GetLightShader()->SetUniform("u_CurrViewProj", _Data.ViewProjectionMatrix);
		_DeferredRendering->GetLightShader()->SetUniform("u_Projection", _Data.ProjectionMatrix);
		_DeferredRendering->GetLightShader()->SetUniform("u_View", _Data.ViewMatrix);
		_DeferredRendering->GetLightShader()->SetUniform("u_InverseProjection", glm::inverse(_Data.ProjectionMatrix));
	}

	void TARE::EndFrame()
	{
		_DeferredRendering->UnbindGBuffer();
		_DeferredRendering->RenderLightingPass(_Data.Lights, _Data.CameraPosition);
	}

	void TARE::BeginShadowPass()
	{
		if (_Data.Lights.empty()) return;

		_ShadowMap->BeginRender(_Data.LightViewProjectionMatrix);
	}

	void TARE::EndShadowPass()
	{
		_ShadowMap->EndRender();
		RenderCommand::SetViewport(0, 0, _Width, _Height);
	}

	void TARE::DrawGrid()
	{
		if (_Data.DrawGrid && DEBUG_RENDERER_USE_GRID)
			_Grid->Render(_Data.ViewMatrix, _Data.ProjectionMatrix, _Data.CameraPosition);
	}

	void TARE::SetLights(const std::vector<Light>& lights)
	{
		if (lights.empty()) return;
		_DeferredRendering->GetLightShader()->Use();
		_DeferredRendering->GetLightShader()->SetUniform("u_LightCount", (int)lights.size());
		for (size_t i = 0; i < lights.size(); ++i) {
			CalculateLight(lights[i], static_cast<int>(i));
		}
		_Data.Lights = lights;
	}

	void TARE::CalculateLight(const Light& light, int index)
	{
		_DeferredRendering->GetLightShader()->Use();
		std::string baseUniform = "u_Lights[" + std::to_string(index) + "]";
		_DeferredRendering->GetLightShader()->SetUniform((baseUniform + ".type").c_str(), static_cast<int>(light.type));
		_DeferredRendering->GetLightShader()->SetUniform((baseUniform + ".position").c_str(), light.position);
		_DeferredRendering->GetLightShader()->SetUniform((baseUniform + ".direction").c_str(), light.direction);
		_DeferredRendering->GetLightShader()->SetUniform((baseUniform + ".color").c_str(), light.color);
		_DeferredRendering->GetLightShader()->SetUniform((baseUniform + ".intensity").c_str(), light.intensity);
		_DeferredRendering->GetLightShader()->SetUniform((baseUniform + ".range").c_str(), light.range);
		_DeferredRendering->GetLightShader()->SetUniform((baseUniform + ".innerCone").c_str(), light.innerCone);
		_DeferredRendering->GetLightShader()->SetUniform((baseUniform + ".outerCone").c_str(), light.outerCone);

		if (light.type == LightType::DIRECTIONAL) {
			float shadowRange = 20.0f;
			glm::mat4 lightProjection = glm::ortho(-shadowRange, shadowRange, -shadowRange, shadowRange, 1.0f, 150.0f);

			glm::vec3 lightPos = -light.direction * 30.0f;
			glm::mat4 lightView = glm::lookAt(
				lightPos,
				glm::vec3(0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);

			_Data.LightViewProjectionMatrix = lightProjection * lightView;
		}

	}
}