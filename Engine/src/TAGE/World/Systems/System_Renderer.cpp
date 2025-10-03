#include "tagepch.h"
#include "System_Renderer.h"

#include "TAGE/World/Scene/Scene.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/World/Components/RenderComponents.h"

namespace TAGE {
	System_Renderer::System_Renderer()
	{
	}

	void System_Renderer::Update(float deltaTime)
	{
		if (!_Scene) return;
		auto& primaryCameraEntity = _Scene->GetPrimaryCamera();
		if (!primaryCameraEntity) return;
		_DeltaTime = deltaTime;

		MEM::Ref<TARE::Camera> camera = nullptr;
		auto& cc = primaryCameraEntity.GetComponent<CameraComponent>();
		auto& tc = _Scene->GetWorldSpaceTransform(primaryCameraEntity);
		if (!cc.Handle) { LOG_ERROR("Primary camera does not have a camera handle."); return; }
		cc.Handle->SetPosition(tc.Position);
		cc.Handle->SetEulerRotation(tc.GetRotationEuler());
		camera = cc.Handle;

		Render(camera, deltaTime, true);
	}

	void System_Renderer::UpdateEditor(float deltaTime)
	{
		if (!_Scene) return;
		if (!_EditorCamera) return;
		_DeltaTime = deltaTime;

		Render(_EditorCamera, deltaTime);
	}

	void System_Renderer::Render(const MEM::Ref<TARE::Camera>& cam, float dt, bool runtime)
	{
		if (!cam) return;
		CollectLights();

		TARE::TARE3D::BeginDeferredRender(cam);
		RenderObjects(runtime);
		TARE::TARE3D::EndDeferredRender();
	}

	void System_Renderer::RenderObjects(bool runtime)
	{
		auto view = _Scene->GetEntitiesWith<MeshComponent>();
		for (auto entity : view) {
			Entity& entityObj = _Scene->GetEntityByID(entity);
			const auto& mc = entityObj.GetComponent<MeshComponent>();

			if (!mc.IsVisible) continue;
			if (!mc.Handle) continue;

			auto& tc = entityObj.GetComponent<TransformComponent>();
			glm::mat4 transform = _Scene->GetWorldSpaceTransformMatrix(entityObj);
			TARE::TARE3D::DrawEntityStaticMesh(mc.Handle, transform, (int)entity);
		}
	}

	void System_Renderer::CollectLights()
	{
		{
			auto& LightEnviroment = TARE::TARE3D::GetLightEnviroment();
			{
				auto lights = _Scene->GetRegistry().group<DirectionalLightComponent>(entt::get<TransformComponent>);
				uint dirLightCount = 0;
				for (auto entity : lights) {
					if (dirLightCount >= MAX_DIRECTIONAL_LIGHTS) break;

					Entity& entityObj = _Scene->GetEntityByID(entity);
					const auto& dlc = entityObj.GetComponent<DirectionalLightComponent>();
					auto& tc = _Scene->GetWorldSpaceTransform(entityObj);

					DirectionalLight dirLight;
					dirLight.Direction = glm::normalize(-tc.GetRotationEuler());
					dirLight.Color = dlc.Color;
					dirLight.Intensity = dlc.Intensity;
					dirLight.CastsShadows = dlc.CastShadows;
					LightEnviroment.DirectionalLight[dirLightCount++] = dirLight;
				}
			}
			{
				auto lights = _Scene->GetRegistry().group<PointLightComponent>(entt::get<TransformComponent>);
				uint pointLightCount = 0;
				LightEnviroment.PointLights.resize(lights.size());
				for (auto entity : lights) {
					if (pointLightCount >= MAX_POINT_LIGHTS) break;

					Entity& entityObj = _Scene->GetEntityByID(entity);
					const auto& plc = entityObj.GetComponent<PointLightComponent>();
					auto& tc = _Scene->GetWorldSpaceTransform(entityObj);
					PointLight pointLight;
					pointLight.Position = tc.Position;
					pointLight.Color = plc.Color;
					pointLight.Intensity = plc.Intensity;
					pointLight.Radius = plc.Radius;
					pointLight.Falloff = plc.Falloff;
					pointLight.CastsShadows = plc.CastShadows;
					LightEnviroment.PointLights[pointLightCount++] = pointLight;
				}
			}
			{
				auto lights = _Scene->GetRegistry().group<SpotLightComponent>(entt::get<TransformComponent>);
				uint spotLightCount = 0;
				LightEnviroment.SpotLights.resize(lights.size());
				for (auto entity : lights) {
					if (spotLightCount >= MAX_SPOT_LIGHTS) break;

					Entity& entityObj = _Scene->GetEntityByID(entity);
					const auto& slc = entityObj.GetComponent<SpotLightComponent>();
					auto& tc = _Scene->GetWorldSpaceTransform(entityObj);
					SpotLight spotLight;
					spotLight.Position = tc.Position;
					spotLight.Direction = glm::normalize(-tc.GetRotationEuler());
					spotLight.Color = slc.Color;
					spotLight.Intensity = slc.Intensity;
					spotLight.Range = slc.Range;
					spotLight.Falloff = slc.Falloff;
					spotLight.CastsShadows = slc.CastShadows;
					LightEnviroment.SpotLights[spotLightCount++] = spotLight;
				}
			}
		}
	}
}