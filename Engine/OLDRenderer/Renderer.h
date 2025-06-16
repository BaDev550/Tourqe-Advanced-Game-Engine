#pragma once
#include "GLAD/glad.h"
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Memory.h"

#include "Camera/Camera.h"
#include "Shader/Shader.h"
#include "Model/Model.h"
#include "Model/EndlessGrid.h"

#include "RenderPass/PostProcessPass.h"
#include "RenderPass/BloomPass.h"
#include "RenderPass/ShadowPass.h"
#include "Deferred/DeferredRendering.h"

#include "Skybox/Skybox.h"

namespace TAGE::Renderer {
	struct SceneData
	{
		glm::vec3 CameraPosition;
		glm::vec3 CameraDirection;
		glm::vec3 CameraUp;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewProjectionMatrix;
		glm::mat4 LightViewProjectionMatrix;
		std::vector<Light> Lights;

		bool DrawGrid = true;
	};

	class GL_Renderer
	{
	public:
		GL_Renderer(int width, int height);

		void Shutdown();
		void BeginFrame(const MEM::Ref<Camera>& cam);
		void BeginGBuffer(const MEM::Ref<Camera>& cam);
		void EndGBuffer();
		void BeginShadowMap();
		void EndShadowMap();
		void EndFrame();
		void CalculateLight(Light& light, int index);
		void SetLights(std::vector<Light>& lights);

		void SetViewport(int x, int y, int width, int height) { _Width = width; _Height = height; glViewport(x, y, width, height); }
		void SetClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }
		void Clear(int clear) { glClear(clear); }

		SceneData& GetSceneData() { return _SceneData; }
		ShadingMode GetShadingMode() const { return _Mode; }
		DebugRenderMode GetDebugRenderMode() const { return _DebugMode; }
		ShadowPass* GetShadowPass() const { return _ShadowPass.get(); }
		BloomPass* GetBloomPass() const { return _BloomPass.get(); }
		PostProcessPass* GetPostProcessPass() const { return _PostProcessPass.get(); }
		DeferredRendering* GetDeferredRenderer() const { return _Deferred.get(); }
	private:
		int _Width;	
		int _Height;
		SceneData _SceneData;
		ShadingMode _Mode;
		RenderMode _RenderMode;
		DebugRenderMode _DebugMode;
		EndlessGrid _EndlessGrid;

		MEM::Scope<DeferredRendering> _Deferred;
		MEM::Scope<PostProcessPass> _PostProcessPass;
		MEM::Scope<BloomPass> _BloomPass;
		MEM::Scope<ShadowPass> _ShadowPass;
		MEM::Scope<Skybox> _SkyBox;

		MEM::Ref<Shader> _MainShader;
		MEM::Ref<Shader> _GridShader;
		MEM::Ref<Shader> _SkyboxShader;

	public:
		void SetShadingMode(ShadingMode mode)
		{
			_Mode = mode;
			switch (mode)
			{
			case ShadingMode::SHADED: return;
			case ShadingMode::WIREFRAME: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); return;
			default:
				break;
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		void SetDebugRenderMode(DebugRenderMode mode)
		{
			_DebugMode = mode;
			switch (mode)
			{
			case DebugRenderMode::NONE: return;
			case DebugRenderMode::WIREFRAME: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); return;
			case DebugRenderMode::BOUNDING_BOX: return;
			case DebugRenderMode::BONES: return;
			case DebugRenderMode::GRID: _SceneData.DrawGrid = !_SceneData.DrawGrid; return;
			default:
				break;
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	};
}