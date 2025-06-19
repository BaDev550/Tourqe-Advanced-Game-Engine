#include "tagepch.h"
#include "System_Renderer.h"
#include "TAGE/World/Scene/Scene.h"
#include "TAGE/World/Components/RenderComponents.h"

namespace TAGE {
	System_Renderer::System_Renderer(TARE::TARE* renderer)
	{
		_Renderer = renderer;
	}

	void System_Renderer::Update(SystemUpdateType updateType, float deltaTime)
	{
		if (_Scene) {
			std::vector<Light> lights;
			MEM::Ref<TARE::Camera> camera = nullptr;
			int lightIndex = 0;

			for (const auto& [id, object] : _Scene->GetObjects()) {
				if (object->HasComponent<CameraComponent>()) {
					const auto& cc = object->GetComponent<CameraComponent>();
					camera = cc->Handle;
				}

				if (object->HasComponent<LightComponent>()) {
					const auto& lc = object->GetComponent<LightComponent>();
					const auto& transform = object->GetComponent<TransformComponent>();

					lc->Handle.position = transform->Position;
					lc->Handle.direction = glm::normalize(glm::rotate(transform->Rotation, glm::vec3(0.0f, 0.0f, -1.0f)));

					lights.push_back(lc->Handle);
					lightIndex++;
				}
			}
			_Renderer->SetLights(lights);

			if (!camera) return;

			TARE::RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
			TARE::RenderCommand::Clear(COLOR_DEPTH);

			_Renderer->BeginShadowPass();
			RenderObjects("ShadowDepth");
			_Renderer->EndShadowPass();

			_Renderer->BeginFrame(camera);
			RenderObjects("GBufferShader");
			_Renderer->EndFrame();

			lights.clear();
		}
	}

	void System_Renderer::RenderObjects(const std::string& shaderName)
	{
		auto view = _Scene->GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto entity : view) {
			Object* object = _Scene->GetObjectByID(entity);
			auto mc = object->GetComponent<MeshComponent>();
			mc->Handle->SetTransform(object->GetTransformMatrix());

			if (mc->IsVisible)
				mc->Handle->Draw(shaderName);
		}
	}
}