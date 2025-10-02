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
		TARE::TARE3D::BeginForwardRender(cam);
		TARE::TARE3D::EndForwardRender();

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
}