//#pragma once
//#include "TAGE/Utilities/Memory.h"
//#include "TAGE/Renderer/Model/Model.h"
//#include "TAGE/Renderer/Camera/Camera.h"
//#include "TAGE/Common/TTypes.h"
//namespace TAGE {
//	using namespace Renderer;
//
//	struct MeshComponent {
//		MEM::Ref<Model> Handle;
//		bool IsVisible = true;
//		bool CastShadows = true;
//
//		MeshComponent() = default;
//		MeshComponent(const std::string& modelpath) {
//			Handle = MEM::MakeRef<Model>();
//			Handle->LoadFromFile(modelpath);
//		}
//	};
//
//	struct LightComponent {
//		Light Handle;
//
//		LightComponent(Light light) : Handle(light) {}
//	};
//
//	struct CameraComponent {
//		MEM::Ref<Camera> Handle;
//
//		CameraComponent() = default;
//		CameraComponent(const MEM::Ref<Camera>& camera) : Handle(camera) {}
//	};
//}