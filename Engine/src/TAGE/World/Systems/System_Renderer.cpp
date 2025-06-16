#include "tagepch.h"
//#include "System_Renderer.h"
//#include "TAGE/World/Scene/Scene.h"
//#include "TAGE/World/Components/RenderComponents.h"
//
//namespace TAGE {
//	System_Renderer::System_Renderer(GL_Renderer* renderer)
//	{
//		_Renderer = renderer;
//	}
//
//	void System_Renderer::Update(SystemUpdateType updateType, float deltaTime)
//	{
//		if (_Scene) {
//			std::vector<Light> lights;
//			MEM::Ref<Camera> camera = nullptr;
//			int lightIndex = 0;
//
//			for (const auto& [id, object] : _Scene->GetObjects()) {
//				if (object->HasComponent<CameraComponent>()) {
//					auto cc = object->GetComponent<CameraComponent>();
//					camera = cc->Handle;
//				}
//
//				if (object->HasComponent<LightComponent>()) {
//					auto lc = object->GetComponent<LightComponent>();
//
//					_Renderer->CalculateLight(lc->Handle, lightIndex);
//					lights.push_back(lc->Handle);
//					lightIndex++;
//				}
//			}
//			_Renderer->SetLights(lights);
//
//			if (!camera) return;
//
//			_Renderer->BeginShadowMap();
//			RenderObjects("ShadowShader");
//			_Renderer->EndShadowMap();
//
//			_Renderer->BeginGBuffer(camera);
//			RenderObjects();
//			_Renderer->EndGBuffer();
//
//			_Renderer->BeginFrame(camera);
//			RenderObjects();
//			_Renderer->EndFrame();
//
//			lights.clear();
//		}
//	}
//
//	void System_Renderer::RenderObjects(const std::string& shaderName)
//	{
//		auto view = _Scene->GetRegistry().view<TransformComponent, MeshComponent>();
//		for (auto entity : view) {
//			Object* object = _Scene->GetObjectByID(entity);
//			auto mc = object->GetComponent<MeshComponent>();
//			mc->Handle->SetTransform(object->GetTransformMatrix());
//
//			if (mc->IsVisible)
//				mc->Handle->Draw(shaderName);
//		}
//	}
//}
//
////_Floor->SetTransform(floor);
////_Asset->SetTransform(glm::mat4(1.0f));
////
////_Renderer->CalculateLights(lightManager.GetLights());
////_Renderer->BeginShadowMap();
////_Floor->Draw("ShadowShader");
////_Asset->Draw("ShadowShader");
////_Renderer->EndShadowMap();
////
////_Renderer->BeginGBuffer(_FreeCam->GetCamera());
////_Floor->Draw("DeferredShader");
////_Asset->Draw("DeferredShader");
////_Renderer->EndGBuffer();
////
////_Renderer->BeginFrame(_FreeCam->GetCamera());
////_Floor->Draw("MainShader");
////_Asset->Draw("MainShader");
////_Renderer->EndFrame();