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
		MEM::Ref<TARE::Camera> camera = nullptr;

		auto& cc = primaryCameraEntity.GetComponent<CameraComponent>();
		auto& tc = primaryCameraEntity.GetComponent<TransformComponent>();
		cc.Handle->SetPosition(tc.Position);
		cc.Handle->SetEulerRotation(glm::eulerAngles(tc.Rotation));
		camera = cc.Handle;

		if (!camera) return;

		Render(camera);
	}

	void System_Renderer::UpdateEditor(float deltaTime)
	{
		if (!_Scene) return;
		if (!_EditorCamera) return;

		Render(_EditorCamera);
	}

	void System_Renderer::Render(const MEM::Ref<TARE::Camera>& cam)
	{
		TARE::RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		TARE::RenderCommand::Clear(COLOR_DEPTH_STENCIL);

		std::vector<Light> lights;
		MEM::Ref<TARE::Skybox> skybox = nullptr;
		GetLights(lights, skybox);

		_Renderer->SetLights(lights);

		_Renderer->BeginShadowPass();
		RenderShadowObject();
		_Renderer->EndShadowPass();

		_Renderer->BeginFrame(cam);
		if (skybox)
			skybox->Bind(cam->GetViewMatrix(), cam->GetProjectionMatrix());
		_Renderer->DrawGrid();
		RenderObjects();
		_Renderer->EndFrame();
	}

	void System_Renderer::RenderObjects()
	{
		auto view = _Scene->GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto entity : view) {
			Entity* entityObj = _Scene->GetEntityByID(entity);
			auto& mc = entityObj->GetComponent<MeshComponent>();
			auto& tc = entityObj->GetComponent<TransformComponent>();
			if (!mc.Handle) continue;

			auto& transform = entityObj->GetComponent<TransformComponent>();

			mc.Handle->SetTransform(transform.GetTransform());

			if (mc.IsVisible) {
				_Renderer->GetDeferredRendering().GetGBufferShader()->Use();
				_Renderer->GetDeferredRendering().GetGBufferShader()->SetUniform("u_EntityID", (int)entity);
				mc.Handle->Draw("GBufferShader");
			}

			if (mc.IsSelected) {
				TARE::RenderCommand::ToggleStencilFunc(true);
				TARE::RenderCommand::Disable(DEPTH_TEST);
				mc.Handle->SetTransform(transform.GetTransform() * 1.05f); 
				mc.Handle->Draw("SingleColor");
				TARE::RenderCommand::ToggleStencilFunc(false);
				TARE::RenderCommand::Enable(DEPTH_TEST);
			}
		}

	}

	void System_Renderer::RenderShadowObject()
	{
		auto view = _Scene->GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto entity : view) {
			Entity* entityObj = _Scene->GetEntityByID(entity);
			auto& mc = entityObj->GetComponent<MeshComponent>();
			if (!mc.Handle) continue;

			auto& transform = entityObj->GetComponent<TransformComponent>();
			mc.Handle->SetTransform(transform.GetTransform());

			if (mc.CastShadows)
				mc.Handle->Draw("ShadowDepth");
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