#pragma once
#include <unordered_map>
#include <queue>
#include <filesystem>
#include "TARE/Buffers/Framebuffer.h"
#include "TARE/Model/Model.h"
#include "TARE/Camera/Camera.h"

namespace TAGE::Editor {
	struct ModelPreview {
		MEM::Ref<TARE::Framebuffer> Framebuffer;
		MEM::Ref<TARE::Model> LoadedModel;
		bool IsRendered = false;
	};

	class ThumbnailRenderer
	{
	public:
		ThumbnailRenderer();
		~ThumbnailRenderer() = default;

		void SubmitModel(const std::filesystem::path& modelPath);
		void OnUpdate();
		uint GetThumbnail(const std::filesystem::path& modelPath);

	private:
		void RenderThumbnail(const std::filesystem::path& modelPath);
		void ComputeCameraForModel(TARE::Model& model);
	private:
		std::unordered_map<std::filesystem::path, ModelPreview> _Cache;
		std::queue<std::filesystem::path> _PendingModels;
		TARE::Camera _ThumbnailCamera;

		MEM::Ref<TARE::Shader> _ThumbnailShader;
		MEM::Ref<TARE::Texture2D> _FallbackTextureIcon;
	};
}