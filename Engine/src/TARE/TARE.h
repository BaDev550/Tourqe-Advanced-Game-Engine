#pragma once
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Memory.h"
#include "Shader/ShaderLibrary.h"
#include "Common/RenderCommands.h"
#include "Model/EndlessGrid.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Buffers/Framebuffer.h"
#include "Deferred/DeferredRendering.h"
#include "Shadow/ShadowMap.h"
#include "Skybox/Skybox.h"
#include "DebugRenderer/DebugLightRenderer.h"
#include "Common/Light.h"

namespace TARE {
	class TARE
	{
		struct CameraData {
			glm::mat4 ViewMatrix;               
			glm::mat4 ProjectionMatrix;         
			glm::mat4 InversedProjectionMatrix; 
			glm::mat4 InversedViewMatrix;       
			glm::mat4 PrevViewProjMatrix;       
			glm::mat4 ViewProjectionMatrix;     

			glm::vec3 CameraPosition; 
			glm::vec3 CameraDirection;
			glm::vec3 CameraUp;

			float farPlane = 1000.0f;
			float nearPlane = 0.1f;
		};

		struct LightsData {
			std::vector<Light> Lights;
		};

		struct ScreenSpaceData {
			int   SS_NumSamples = 15;
			float SS_SampleRadius = 1.5f;
			int   SS_MaxSteps = 1;
			int   SS_StepSize = 15;
			int   SS_Tolerance = 1;
			int	  SS_Distance = 15;
			float SS_FadeStart = 0.1f;
			float SS_FadeEnd = 0.01f;
			int   SS_BinarySearchSteps = 5;
			float SS_Thickness = 1.5f;
			float SS_Intensity = 1.0f;
			float SS_MaxReflectionLOD = 4.0f;
			float SS_GlobalIlluminationIntensity = 3.0f;
		};

		struct SceneData {
			CameraData CameraData;
			LightsData LightData;
			ScreenSpaceData ScreenSpaceData;
			TAGE::MEM::Ref<PostProcess> PostProcess;

			bool UseSSGI = false;
			bool UseSSReflections = false;
		} _Data;
	public:
		TARE(int width, int height);
		void BeginFrame(const TAGE::MEM::Ref<Camera>& cam, const TAGE::MEM::Ref<Skybox>& skybox = nullptr);
		void EndFrame();

		void BeginShadowPass(const TAGE::MEM::Ref<Camera>& cam);
		void EndShadowPass();

		void DrawGrid();
		void Resize(int width, int height);

		void SetLights(const std::vector<Light>& Lights);
		SceneData& GetSceneData() { return _Data; }
		DeferredRendering& GetDeferredRendering() { return *_DeferredRendering; }
		ShadowMap& GetShadowMap() { return *_ShadowMap; }
	private:
		int _Width, _Height;
	private:
		TAGE::MEM::Scope<EndlessGrid> _Grid;
		TAGE::MEM::Ref<DeferredRendering> _DeferredRendering;
		TAGE::MEM::Ref<ShadowMap> _ShadowMap;
	};
}

