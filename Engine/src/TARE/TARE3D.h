#pragma once
#include "Shader/ShaderLibrary.h"
#include "Model/Mesh.h"
#include "Model/Model.h"
#include "Model/Grid.h"
#include "Camera/Camera.h"
#include "Types/Light.h"
#include "Types/EnviromentMap.h"
#include "Buffers/Framebuffer.h"

#define MAX_BONES 124
#define MAX_MATERIALS 32
#define MAX_INSTANCES 1024
#define MAX_BONE_INFLUENCES 4
#define SCALE_FACTOR 0.01f
#define SHADOW_MAP_RESOLUTION 4056

namespace TARE {
	struct SceneLightEnviroment {
		DirectionalLight DirectionalLight[MAX_DIRECTIONAL_LIGHTS];
		std::vector<PointLight> PointLights;
		std::vector<SpotLight> SpotLights;
		_NODISCARD uint GetPointLightCount() const { return (uint)(PointLights.size() * sizeof(PointLight)); }
		_NODISCARD uint GetSpotLightCount() const {  return (uint)(SpotLights.size() * sizeof(SpotLight)); }
	};

	struct CameraUniformBufferData {
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewProjectionMatrix;
		glm::mat4 InversedProjectionMatrix;
		glm::mat4 InversedViewMatrix;

		glm::vec3 CameraPosition;
		glm::vec3 CameraDirection;
		glm::vec3 CameraUp;
	};

	struct SceneData {
		CameraUniformBufferData CameraData;
		SceneLightEnviroment LightEnviroment;
		EnviromentMap* Enviroment = nullptr;
	};

	class TARE3D
	{
	public:
		static void Init();
		static void Destroy();
		static void Resize(uint width, uint height);
		
		static void BeginDeferredRender(const TAGE::MEM::Ref<Camera>& camera);
		static void EndDeferredRender();

		static void DrawStaticMesh(TAGE::MEM::Ref<Model> model, const glm::mat4& transform);
		static void DrawEntityStaticMesh(TAGE::MEM::Ref<Model> model, const glm::mat4& transform, int EntityID);
		static void DrawSkybox();

		static ShaderLibrary& GetShaderLibrary() { return _ShaderLibrary; }
		static SceneData& GetSceneData() { return _SceneData; }
		static SceneLightEnviroment& GetLightEnviroment() { return _SceneData.LightEnviroment; }
		static EnviromentMap& GetEnviromentMap() { return *_SceneData.Enviroment; }
		static bool HasEnviromentMap() { return _SceneData.Enviroment != nullptr; }

		static TAGE::MEM::Ref<Framebuffer> GetGBuffer() { return _GBuffer; }
		static TAGE::MEM::Ref<Framebuffer> GetFinalBuffer() { return _FinalBuffer; }

		static void SetEnviromentMap(EnviromentMap* env) { _SceneData.Enviroment = env; }
		static TAGE::MEM::Ref<TARE::TextureCube> EquirectangularToCubemap(TAGE::MEM::Ref<TARE::Texture2D> equirectangularMap);
		static TAGE::MEM::Ref<TARE::TextureCube> CreateIrradianceMap(TAGE::MEM::Ref<TARE::TextureCube> envCubemap);
	private:
		static ShaderLibrary _ShaderLibrary;
		static SceneData _SceneData;

		static UniformBufferDirectionalLight _UBODirectionalLight;
		static UniformBufferPointLights _UBOPointLights;   // to-do : move this variables another class
		static UniformBufferSpotLights _UBOSpotLights;

		static TAGE::MEM::Ref<Framebuffer> _GBuffer;
		static TAGE::MEM::Ref<Framebuffer> _FinalBuffer;
	};
}

