#include "tagepch.h"
#include "System_Renderer.h"
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

		std::vector<Light> lights;
		MEM::Ref<TARE::Camera> camera = nullptr;
		MEM::Ref<TARE::Skybox> skybox = nullptr;
		GetLights(lights, skybox);

		auto& cc = primaryCameraEntity.GetComponent<CameraComponent>();
		auto& tc = primaryCameraEntity.GetComponent<TransformComponent>();
		cc.Handle->SetPosition(tc.Position);
		cc.Handle->SetEulerRotation(glm::eulerAngles(tc.Rotation));
		camera = cc.Handle;

		if (!camera) return;

		_Renderer->SetLights(lights);

		TARE::RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		TARE::RenderCommand::Clear(COLOR_DEPTH);

		_Renderer->BeginShadowPass();
		RenderObjects("ShadowDepth");
		_Renderer->EndShadowPass();

		_Renderer->BeginFrame(camera);
		if (skybox)
			skybox->Bind(camera->GetViewMatrix(), camera->GetProjectionMatrix());
		RenderObjects("GBufferShader");
		_Renderer->EndFrame();
	}

	void System_Renderer::UpdateEditor(float deltaTime)
	{
		if (!_Scene) return;

		std::vector<Light> lights;
		MEM::Ref<TARE::Skybox> skybox = nullptr;
		GetLights(lights, skybox);

		_Renderer->SetLights(lights);

		if (!_EditorCamera) return;

		TARE::RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		TARE::RenderCommand::Clear(COLOR_DEPTH);

		_Renderer->BeginShadowPass();
		RenderObjects("ShadowDepth");
		_Renderer->EndShadowPass();

		_Renderer->BeginFrame(_EditorCamera);
		if (skybox)
			skybox->Bind(_EditorCamera->GetViewMatrix(), _EditorCamera->GetProjectionMatrix());
		_Renderer->DrawGrid();
		RenderObjects("GBufferShader");
		_Renderer->EndFrame();
	}

	void System_Renderer::RenderObjects(const std::string& shaderName)
	{
		auto view = _Scene->GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto entity : view) {
			Entity entityObj = _Scene->GetEntityByID(entity);
			auto& mc = entityObj.GetComponent<MeshComponent>();
			if (!mc.Handle) continue;

			auto& transform = entityObj.GetComponent<TransformComponent>();
			mc.Handle->SetTransform(transform.GetTransform());

			if (mc.IsVisible)
				mc.Handle->Draw(shaderName);
		}
	}

	void System_Renderer::GetLights(std::vector<Light>& lights, MEM::Ref<TARE::Skybox>& skybox)
	{
		{
			auto lightEntities = _Scene->GetEntitiesWith<TransformComponent, LightComponent>();
			for (const auto& entity : lightEntities)
			{
				auto& lc = entity->GetComponent<LightComponent>();
				auto& transform = entity->GetComponent<TransformComponent>();
				lc.Handle.position = transform.Position;
				lc.Handle.direction = glm::normalize(glm::rotate(transform.Rotation, glm::vec3(0.0f, 0.0f, -1.0f)));
				lights.push_back(lc.Handle);
			}
		}
		{
			auto skyboxEntities = _Scene->GetEntitiesWith<SkyboxComponent>();
			for (const auto& entity : skyboxEntities)
			{
				skybox = entity->GetComponent<SkyboxComponent>().Handle;
				break;
			}
		}
	}
}