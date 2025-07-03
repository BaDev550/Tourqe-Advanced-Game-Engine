#include "tagepch.h"
#include "TARE.h"

namespace TARE {
	TARE::TARE(int width, int height)
	{
		ShaderLibrary::Add("MainShader", "../Engine/shaders/main_vertex", "../Engine/shaders/main_fragment");
		ShaderLibrary::Add("OutlineShader", "../Engine/shaders/Common/outline_vertex", "../Engine/shaders/Common/outline_fragment");
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
		_Data.InversedViewMatrix = cam->GetInverseViewMatrix();

		_DeferredRendering->GetLightShader()->Use();
		_DeferredRendering->GetLightShader()->SetUniform("u_CameraPos", _Data.CameraPosition);
		_DeferredRendering->GetLightShader()->SetUniform("u_InverseView", _Data.InversedViewMatrix);

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

	void TARE::SetLights(std::vector<Light>& lights)
	{
		if (lights.empty()) return;

		int count = (int)lights.size();
		_DeferredRendering->GetLightShader()->Use();
		_DeferredRendering->GetLightShader()->SetUniform("u_LightCount", count);

		for (int i = 0; i < MAX_LIGHTS; ++i) {
			if (i < count)
				CalculateLight(lights[i], i);
			else
				ClearLight(i);
		}
		_Data.Lights = lights;
	}

	void TARE::CalculateLight(const Light& light, int index)
	{
        auto shader = _DeferredRendering->GetLightShader();
        shader->Use();

        std::string baseUniform = "u_Lights[" + std::to_string(index) + "]";

        shader->SetUniform((baseUniform + ".type").c_str(), static_cast<int>(light.type));
        shader->SetUniform((baseUniform + ".position").c_str(), light.position);
        shader->SetUniform((baseUniform + ".direction").c_str(), light.direction);
        shader->SetUniform((baseUniform + ".color").c_str(), light.color);
        shader->SetUniform((baseUniform + ".intensity").c_str(), light.intensity);
        shader->SetUniform((baseUniform + ".range").c_str(), light.range);
        shader->SetUniform((baseUniform + ".innerCone").c_str(), light.innerCone);
        shader->SetUniform((baseUniform + ".outerCone").c_str(), light.outerCone);

		if (light.castShadow && light.shadowMap) {
			int textureSlot = SHADOW_MAP_TEXTURE_SLOT + index;
			shader->SetUniform((baseUniform + ".lightSpaceMatrix").c_str(), light.lightSpaceMatrix);

			light.shadowMap->BindTexture(textureSlot);
			shader->SetUniform(("u_ShadowMaps[" + std::to_string(index) + "]").c_str(), textureSlot);
		}
		else {
			ClearShadowLight(index);
		}
	}

	void TARE::ClearLight(int index)
	{
		auto shader = _DeferredRendering->GetLightShader();
		std::string baseUniform = "u_Lights[" + std::to_string(index) + "]";

		shader->SetUniform((baseUniform + ".type").c_str(), -1);
		shader->SetUniform((baseUniform + ".position").c_str(), glm::vec3(0.0f));
		shader->SetUniform((baseUniform + ".direction").c_str(), glm::vec3(0.0f));
		shader->SetUniform((baseUniform + ".color").c_str(), glm::vec3(0.0f));
		shader->SetUniform((baseUniform + ".intensity").c_str(), 0.0f);
		shader->SetUniform((baseUniform + ".range").c_str(), 0.0f);
		shader->SetUniform((baseUniform + ".innerCone").c_str(), 0.0f);
		shader->SetUniform((baseUniform + ".outerCone").c_str(), 0.0f);
		shader->SetUniform((baseUniform + ".lightSpaceMatrix").c_str(), glm::mat4(1.0f));

		shader->SetUniform(("u_ShadowMaps[" + std::to_string(index) + "]").c_str(), 0);
	}

	void TARE::ClearShadowLight(int index)
	{
		auto shader = _DeferredRendering->GetLightShader();
		std::string baseUniform = "u_Lights[" + std::to_string(index) + "]";

		shader->SetUniform((baseUniform + ".lightSpaceMatrix").c_str(), glm::mat4(1.0f));
		shader->SetUniform(("u_ShadowMaps[" + std::to_string(index) + "]").c_str(), 0);
	}
}