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
		cc.Handle->SetPosition(tc.Position);
		cc.Handle->SetEulerRotation(tc.GetRotationEuler());
		camera = cc.Handle;

		if (!camera) return;

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
		TARE::RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		TARE::RenderCommand::Clear(COLOR_DEPTH_STENCIL);

		std::vector<Light> lights;
		MEM::Ref<TARE::Skybox> skybox = nullptr;
		GetLights(lights, skybox);

		RenderShadowPass(lights);
		_Renderer->SetLights(lights);

		_Renderer->BeginFrame(cam);
		if (skybox)
			skybox->Bind(cam->GetViewMatrix(), cam->GetProjectionMatrix());

		if (!runtime)
			_Renderer->DrawGrid();

		RenderObjects();

		if (!runtime && TARE::RenderAPI::GetDebugRenderMode() == DebugRenderMode::BOUNDING_BOX)
			_Scene->GetPhysicsWorld()->DrawDebug(dt, cam->GetViewProjectionMatrix());

		_Renderer->EndFrame();
	}

	void System_Renderer::RenderObjects()
	{
		auto view = _Scene->GetEntitiesWith<MeshComponent>();
		for (auto entity : view) {
			Entity& entityObj = _Scene->GetEntityByID(entity);
			auto& mc = entityObj.GetComponent<MeshComponent>();
			auto& tc = entityObj.GetComponent<TransformComponent>();
			if (!mc.Handle) continue;

			glm::mat4 transform = _Scene->GetWorldSpaceTransformMatrix(entityObj);
			mc.Handle->SetTransform(transform);

			if (mc.IsVisible) {
				TARE::RenderCommand::ToggleStencilFunc(StencilMode::ENABLE);
				_Renderer->GetDeferredRendering().GetGBufferShader()->Use();
				_Renderer->GetDeferredRendering().GetGBufferShader()->SetUniform("u_EntityID", (int)entity);
				mc.Handle->Draw("GBufferShader");

				if (_EditorCamera && mc.IsSelected) {
					TARE::RenderCommand::ToggleStencilFunc(StencilMode::DISABLE);
					TARE::RenderCommand::Disable(DEPTH_TEST);
					mc.Handle->DrawOutlined("OutlineShader", _EditorCamera->GetViewProjectionMatrix());
					TARE::RenderCommand::ToggleStencilFunc(StencilMode::ENABLE_FOR_READING);
					TARE::RenderCommand::Enable(DEPTH_TEST);
				}
			}
		}

	}

	void System_Renderer::RenderShadowObject(bool point)
	{
		auto view = _Scene->GetEntitiesWith<TransformComponent, MeshComponent>();
		for (auto entity : view) {
			Entity& entityObj = _Scene->GetEntityByID(entity);
			auto& mc = entityObj.GetComponent<MeshComponent>();
			if (!mc.Handle) continue;

			glm::mat4 transform = _Scene->GetWorldSpaceTransformMatrix(entityObj);
			mc.Handle->SetTransform(transform);

			if (mc.CastShadows)
				mc.Handle->Draw(point ? "PointLightShadowDepth" : "ShadowDepth");
		}
	}

	void System_Renderer::RenderShadowPass(std::vector<Light>& lights)
	{
		for (int i = 0; i < lights.size(); i++)
		{
			Light& light = lights[i];
			if (!light.castShadow || !light.shadowMap)
				continue;

			switch (light.type)
			{
			case LightType::DIRECTIONAL:
			{
				float shadowRange = 30.0f;
				glm::mat4 lightProjection = glm::ortho(-shadowRange, shadowRange, -shadowRange, shadowRange, 0.1f, 150.0f);

				glm::vec3 lightPos = -light.direction * 30.0f;
				glm::mat4 lightView = glm::lookAt(
					lightPos,
					glm::vec3(0.0f),
					glm::vec3(0.0f, 1.0f, 0.0f)
				);
				glm::mat4 lightSpaceMatrix = lightProjection * lightView;

				light.shadowMap->BeginRender(lightSpaceMatrix);
				RenderShadowObject();
				light.shadowMap->EndRender();

				light.lightSpaceMatrix = lightSpaceMatrix;
				break;
			}
			case LightType::POINT:
			{
				float nearPlane = 1.0f;
				float farPlane = light.range;
				float fov = glm::radians(90.0f);

				glm::mat4 shadowProj = glm::perspective(fov, (float)SHADOW_MAP_RESOLUTION / (float)SHADOW_MAP_RESOLUTION, nearPlane, farPlane);

				std::vector<glm::mat4> shadowTransforms;
				glm::vec3 pos = light.position;

				shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(1, 0, 0),  glm::vec3(0, -1, 0))); // +X
				shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0))); // -X
				shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 1, 0),  glm::vec3(0, 0, 1))); // +Y
				shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1))); // -Y
				shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 0, 1),  glm::vec3(0, -1, 0))); // +Z
				shadowTransforms.push_back(shadowProj * glm::lookAt(pos, pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))); // -Z

				light.shadowMap->BeginPointLightShadowPass(pos);
				for (int i = 0; i < 6; ++i) {
					light.shadowMap->SetFaceViewProjection(i, shadowTransforms[i]);
					RenderShadowObject(true);
				}
				light.shadowMap->EndPointLightShadowPass();

				break;
			}
			case LightType::SPOT:
			{
				float nearPlane = 1.0f;
				float farPlane = light.range;
				float fov = glm::degrees(light.outerCone) * 2.0f;

				glm::mat4 lightProjection = glm::perspective(glm::radians(fov), 1.0f, nearPlane, farPlane);
				glm::mat4 lightView = glm::lookAt(light.direction, light.position, glm::vec3(0, 1, 0));
				glm::mat4 lightSpaceMatrix = lightProjection * lightView;

				light.shadowMap->BeginRender(lightSpaceMatrix);
				RenderShadowObject();
				light.shadowMap->EndRender();

				light.lightSpaceMatrix = lightSpaceMatrix;
				break;
			}
			default:
				return;
			}
		}
	}

	void System_Renderer::AssignShadowsMaps(std::vector<Light>& lights)
	{
		for (auto& light : lights)
		{
			if (light.castShadow && !light.shadowMap)
			{
				light.shadowMap = TAGE::MEM::MakeRef<TARE::ShadowMap>(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
			}
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