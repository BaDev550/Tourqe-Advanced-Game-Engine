#include "tagepch.h"
#include "TARE.h"

namespace TARE {
	TARE::TARE(int width, int height)
	{
		_Width = width;
		_Height = height;

		_Grid = TAGE::MEM::MakeScope<EndlessGrid>();
		_DeferredRendering = TAGE::MEM::MakeRef<DeferredRendering>(width, height);
		_ShadowMap = TAGE::MEM::MakeRef<ShadowMap>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

		_DeferredRendering->GetLightShader()->CreateUBO("CameraUBO", sizeof(_Data.CameraData), 0);
		_DeferredRendering->GetLightShader()->CreateSSBO(1, sizeof(Light) * MAX_LIGHTS + sizeof(int));
		_DeferredRendering->GetLightShader()->CreateUBO("ScreenSpaceUBO", sizeof(_Data.ScreenSpaceData),  2);
	}

	void TARE::BeginFrame(const TAGE::MEM::Ref<Camera>& cam, const TAGE::MEM::Ref<Skybox>& skybox)
	{
		_Data.CameraData.PrevViewProjMatrix = _Data.CameraData.ViewProjectionMatrix;
		_Data.CameraData.CameraPosition = cam->GetPosition();
		_Data.CameraData.CameraDirection = cam->GetForward();
		_Data.CameraData.CameraUp = cam->GetUp();
		_Data.CameraData.ViewMatrix = cam->GetViewMatrix();
		_Data.CameraData.ProjectionMatrix = cam->GetProjectionMatrix();
		_Data.CameraData.ViewProjectionMatrix = cam->GetViewProjectionMatrix();
		_Data.CameraData.InversedViewMatrix = cam->GetInverseViewMatrix();
		_Data.CameraData.InversedProjectionMatrix = cam->GetInverseProjectionMatrix();
		_Data.CameraData.farPlane = cam->GetFarClip();
		_Data.CameraData.nearPlane = cam->GetNearClip();

		_DeferredRendering->GetLightShader()->Use();
		_DeferredRendering->GetLightShader()->UpdateUBO(0, &_Data.CameraData, sizeof(_Data.CameraData));
		_DeferredRendering->GetLightShader()->UpdateUBO(2, &_Data.ScreenSpaceData, sizeof(_Data.ScreenSpaceData));
		_DeferredRendering->GetLightShader()->SetUniform("u_SceneData.useSSGI", _Data.UseSSGI);
		_DeferredRendering->GetLightShader()->SetUniform("u_SceneData.UseSSR",  _Data.UseSSReflections);

		_DeferredRendering->RenderGeometryPass(cam);

		if (cam->GetPostProcess()) { _Data.PostProcess = cam->GetPostProcess(); }
		if (skybox) { skybox->Bind(cam->GetViewMatrix(), cam->GetProjectionMatrix()); }
	}

	void TARE::EndFrame()
	{
		_DeferredRendering->UnbindGBuffer();
		_DeferredRendering->RenderLightingPass(_Data.Lights, _Data.CameraData.CameraPosition);

		_Data.PostProcess->Render(_DeferredRendering->GetLightingBuffer());
	}

	void TARE::BeginShadowPass(const TAGE::MEM::Ref<Camera>& cam)
	{
		if (_Data.Lights.empty()) return;
		glm::vec3 lightDir;

		for (size_t i = 0; i < _Data.Lights.size(); ++i) {
			if (_Data.Lights[i].type == LightType::DIRECTIONAL) {
				lightDir = _Data.Lights[i].direction;
			}
		}
		_ShadowMap->BeginRender(cam, lightDir);
	}

	void TARE::EndShadowPass()
	{
		_ShadowMap->EndRender();
		RenderCommand::SetViewport(0, 0, _Width, _Height);

		_DeferredRendering->GetLightShader()->Use();
		RenderCommand::BindTextureArrayFromID(_ShadowMap->GetTextureID(), SHADOW_MAP_TEXTURE_SLOT);
		_DeferredRendering->GetLightShader()->SetUniform("u_ShadowMap",   SHADOW_MAP_TEXTURE_SLOT);
		_DeferredRendering->GetLightShader()->SetUniform("u_CascadeCount", (int)_ShadowMap->GetShadowCascadeLevels().size());
		for (size_t i = 0; i < _ShadowMap->GetShadowCascadeLevels().size(); ++i) {
			_DeferredRendering->GetLightShader()->SetUniform(("u_CascadePlaneDistances[" + std::to_string(i) + "]").c_str(), _ShadowMap->GetShadowCascadeLevels()[i]);
		}
	}

	void TARE::DrawGrid()
	{
		_Grid->Render();
	}

	void TARE::Resize(int width, int height)
	{
		_Width = width;
		_Height = height;
		_DeferredRendering->GetGBuffer()->Resize(width, height);
		_DeferredRendering->GetGIBuffer()->Resize(width, height);
		_DeferredRendering->GetLightingBuffer()->Resize(width, height);
		if (_Data.PostProcess) {
			_Data.PostProcess->Resize(width, height);
		}
	}

	void TARE::SetLights(std::vector<Light>& lights)
	{
		if (lights.empty()) return;
		int index = 0;

		for (auto it : lights) {
			_Data.LightData.Lights[index] = it;
			index++;
		}
		_Data.Lights = lights;
		_Data.LightData.LightCount = index;
		index = 0;

		auto shader = _DeferredRendering->GetLightShader();
		shader->Use();
		_DeferredRendering->GetLightShader()->UpdateSSBO(1, lights.data(), sizeof(Light) * _Data.LightData.LightCount + sizeof(int));
	}
}