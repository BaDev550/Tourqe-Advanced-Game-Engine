#include "ThumbnailRenderer.h"
#include "TARE/Common/RenderCommands.h"
#include "TARE/Shader/ShaderLibrary.h"

namespace TAGE::Editor {
	ThumbnailRenderer::ThumbnailRenderer() {
		_FallbackTextureIcon = TARE::Texture2D::Create();
		_FallbackTextureIcon->LoadTexture("assets/textures/Icons/File.png");
		_ThumbnailShader = TARE::ShaderLibrary::Add("ThumbnailShader", "../Engine/shaders/thumbnail_vertex", "../Engine/shaders/thumbnail_fragment");
	}

	void ThumbnailRenderer::SubmitModel(const std::filesystem::path& modelPath)
	{
		if (_Cache.find(modelPath) == _Cache.end())
			_PendingModels.push(modelPath);
	}

	void ThumbnailRenderer::OnUpdate()
	{
#if 0
		if (!_PendingModels.empty())
		{
			const auto& modelPath = _PendingModels.front();
			RenderThumbnail(modelPath);
			_PendingModels.pop();
		}
#endif
	}

	uint ThumbnailRenderer::GetThumbnail(const std::filesystem::path& modelPath)
	{
		if (_Cache.find(modelPath) != _Cache.end() && _Cache[modelPath].IsRendered)
			return _Cache[modelPath].Framebuffer->GetColorAttachment(0);

		return _FallbackTextureIcon->GetID();
	}
	void ThumbnailRenderer::RenderThumbnail(const std::filesystem::path& modelPath)
	{
#if 0
		ModelPreview preview;
		static bool ModelLoaded = false;
		preview.LoadedModel = MEM::MakeRef<TARE::Model>();
		preview.LoadedModel->LoadModelAsync(modelPath.string(), [&](MEM::Ref<TARE::Model> model) {
			if (model) { 
				preview.LoadedModel = std::move(model);
				ComputeCameraForModel(*preview.LoadedModel);
				ModelLoaded = true;
			}
			else { LOG_ERROR("Failed to load async model"); }
			});

			FramebufferSpecification spec(
				FramebufferAttachmentSpecification({ FramebufferTextureFormat::RGBA8 }),
				1,
				64, 64
			);
			preview.Framebuffer = TARE::Framebuffer::Create(spec);

			preview.Framebuffer->Bind();
			TARE::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			TARE::RenderCommand::Clear(COLOR_DEPTH);

			_ThumbnailShader->Use();
			_ThumbnailShader->SetUniform("u_View", _ThumbnailCamera.GetViewMatrix());
			_ThumbnailShader->SetUniform("u_Projection", _ThumbnailCamera.GetProjectionMatrix());

			preview.LoadedModel->SetTransform(glm::mat4(1.0f));
			preview.LoadedModel->Draw(_ThumbnailShader);

			preview.Framebuffer->Unbind();
			preview.IsRendered = true;
			_Cache[modelPath] = preview;
#endif
	}

	void ThumbnailRenderer::ComputeCameraForModel(TARE::Model& model)
	{
		auto bounds = model.GetBoundingBox();
		glm::vec3 center = (bounds.Min + bounds.Max) * 0.5f;
		glm::vec3 extents = bounds.Max - bounds.Min;

		float objectSize = glm::max(glm::max(extents.x, extents.y), extents.z);
		if (objectSize < 0.01f)
			objectSize = 0.5f;

		float verticalFOV = glm::radians(_ThumbnailCamera.GetFOV());

		float cameraViewHeight = 2.0f * glm::tan(verticalFOV * 0.5f);
		float cameraDistanceScalar = 1.0f;

		float distance = cameraDistanceScalar * objectSize / cameraViewHeight;
		distance += 0.5f * objectSize;

		glm::vec3 cameraForward = glm::normalize(glm::vec3(1.0f, -0.3f, -0.7f));
		glm::vec3 cameraPos = center - distance * cameraForward;

		_ThumbnailCamera.SetPosition(cameraPos);
		_ThumbnailCamera.LookAt(center);
	}
}