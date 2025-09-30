#pragma once
#include "Shader/ShaderLibrary.h"
#include "Model/Mesh.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#define MAX_LIGHTS 64
#define MAX_BONES 124
#define MAX_MATERIALS 32
#define MAX_INSTANCES 1024
#define MAX_BONE_INFLUENCES 4
#define SCALE_FACTOR 0.01f
#define SHADOW_MAP_RESOLUTION 4056

namespace TARE {
	class TARE3D
	{
	public:
		static void Init();
		static void Destroy();

		static void BeginForwardRender(const TAGE::MEM::Ref<Camera>& camera);
		static void EndForwardRender();

		static void DrawStaticMesh(TAGE::MEM::Ref<Model> model, const glm::mat4& transform);

		static ShaderLibrary& GetShaderLibrary() { return _ShaderLibrary; }
	private:
		static ShaderLibrary _ShaderLibrary;
	};
}

