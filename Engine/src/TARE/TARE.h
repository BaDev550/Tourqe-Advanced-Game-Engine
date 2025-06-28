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
#include "Common/Light.h"

namespace TARE {
	class TARE
	{
		struct SceneData {
			glm::vec3 CameraPosition;
			glm::vec3 CameraDirection;
			glm::vec3 CameraUp;
			glm::mat4 ViewMatrix;
			glm::mat4 InversedViewMatrix;
			glm::mat4 ProjectionMatrix;
			glm::mat4 PrevViewProjMatrix;
			glm::mat4 ViewProjectionMatrix;
			glm::mat4 LightViewProjectionMatrix;
			std::vector<Light> Lights;

			bool DrawGrid = true;
		};
	public:
		TARE(int width, int height);
		void BeginFrame(const TAGE::MEM::Ref<Camera>& cam);
		void EndFrame();

		void BeginShadowPass();
		void EndShadowPass();

		void DrawGrid();

		void Resize(int width, int height) {
			_Width = width;
			_Height = height;
			_DeferredRendering->GetGBuffer()->Resize(width, height);
			_DeferredRendering->GetGIBuffer()->Resize(width, height);
			_DeferredRendering->GetLightingBuffer()->Resize(width, height);
		}

		void SetLights(std::vector<Light>& lights);
		SceneData& GetSceneData() { return _Data; }
		DeferredRendering& GetDeferredRendering() { return *_DeferredRendering; }
		ShadowMap& GetShadowMap() { return *_ShadowMap; }
	private:
		int _Width, _Height;

		void CalculateLight(const Light& light, int index);
		void ClearLight(int index);
		void ClearShadowLight(int index);
	private:
		TAGE::MEM::Scope<EndlessGrid> _Grid;
		TAGE::MEM::Ref<DeferredRendering> _DeferredRendering;
		TAGE::MEM::Ref<ShadowMap> _ShadowMap;
		SceneData _Data;
	};
}

