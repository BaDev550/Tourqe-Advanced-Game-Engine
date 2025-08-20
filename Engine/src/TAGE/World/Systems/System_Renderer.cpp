#include "tagepch.h"
#include "System_Renderer.h"
#include "TARE/Common/RenderAPI.h"
#include "TAGE/World/Scene/Scene.h"
#include "TAGE/World/Objects/Entity.h"
#include "TAGE/World/Components/BaseComponents.h"
#include "TAGE/World/Components/RenderComponents.h"

namespace TAGE {
	System_Renderer::System_Renderer(TARE::TARE* renderer)
	{
		_Renderer = renderer;
	}

	void System_Renderer::Update(float deltaTime)
	{
		if (!_Scene) return;
		auto& primaryCameraEntity = _Scene->GetPrimaryCamera();
		if (!primaryCameraEntity) return;

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

		Render(_EditorCamera, deltaTime);
	}

	void System_Renderer::Render(const MEM::Ref<TARE::Camera>& cam, float dt, bool runtime)
	{
		TARE::RenderCommand::Clear(COLOR_DEPTH);
		TARE::RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });

		std::vector<Light> lights;
		MEM::Ref<TARE::Skybox> skybox = nullptr;
		GetLights(lights, skybox);
		_Renderer->SetLights(lights);

		_Renderer->BeginShadowPass(cam);
		RenderShadowObject();
		_Renderer->EndShadowPass();

		_Renderer->BeginFrame(cam, skybox);
		if (!runtime) {
			_Renderer->DrawGrid();
			//for (auto& light : lights) { 
			//	_Renderer->GetDebugLightRenderer().SetType(light.type);
			//	_Renderer->GetDebugLightRenderer().Render(cam->GetViewProjectionMatrix(), light.position, light.range, 16, light.color); 
			//}
		}
		RenderObjects();
		_Renderer->EndFrame();
	}

	void System_Renderer::RenderObjects()
	{
		auto view = _Scene->GetEntitiesWith<MeshComponent>();
		for (auto entity : view) {
			Entity& entityObj = _Scene->GetEntityByID(entity);
			auto& mc = entityObj.GetComponent<MeshComponent>();

			if (!mc.IsVisible) continue;
			if (!mc.Handle) continue;

			auto& tc = entityObj.GetComponent<TransformComponent>();
			glm::mat4 transform = _Scene->GetWorldSpaceTransformMatrix(entityObj);
			mc.Handle->SetTransform(transform);
			_Renderer->GetDeferredRendering().GetGBufferShader()->Use();
			_Renderer->GetDeferredRendering().GetGBufferShader()->SetUniform("u_EntityID", (int)entity);
			mc.Handle->Draw("GBufferShader");
		}

	}

	void System_Renderer::RenderShadowObject(bool point)
	{
		auto view = _Scene->GetEntitiesWith<TransformComponent, MeshComponent>();
		for (auto entity : view) {
			Entity& entityObj = _Scene->GetEntityByID(entity);
			auto& mc = entityObj.GetComponent<MeshComponent>();

			if (!mc.Handle) continue;
			if (!mc.CastShadows) continue;

			glm::mat4 transform = _Scene->GetWorldSpaceTransformMatrix(entityObj);
			mc.Handle->SetTransform(transform);
			mc.Handle->Draw("ShadowDepth");
		}
	}

	void System_Renderer::GetLights(std::vector<Light>& lights, MEM::Ref<TARE::Skybox>& skybox)
	{
		{
			auto lightEntities = _Scene->GetEntitiesWith<LightComponent>();
			for (const auto& entity : lightEntities)
			{
				Entity& lightEntity = _Scene->GetEntityByID(entity);
				auto& lc = lightEntity.GetComponent<LightComponent>();
				auto& transform = lightEntity.GetComponent<TransformComponent>();
				lc.Handle.position = transform.Position;
				lc.Handle.direction = transform.GetRotationEuler();
				lights.push_back(lc.Handle);
			}
		}
		{
			auto skyboxEntities = _Scene->GetEntitiesWith<SkyboxComponent>();
			for (const auto& entity : skyboxEntities)
			{
				Entity& skyBoxEntity = _Scene->GetEntityByID(entity);
				skybox = skyBoxEntity.GetComponent<SkyboxComponent>().Handle;
				break;
			}
		}
	}
}