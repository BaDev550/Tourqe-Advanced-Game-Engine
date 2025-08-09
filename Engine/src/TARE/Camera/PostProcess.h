#pragma once
#include "TARE/Buffers/Framebuffer.h"
#include "TARE/Shader/Shader.h"

namespace TARE {
	struct PostProcessSettings {
		int AA_Samples;
		float Exposure;

		PostProcessSettings(int aaSamples = 4, float exposure = 1.0f)
			: AA_Samples(aaSamples), Exposure(exposure) {
		}
	};

	class PostProcess
	{
	public:
		PostProcess(int width, int height, PostProcessSettings settings);
		~PostProcess() = default;

		void Render(const TAGE::MEM::Ref<Framebuffer>& source);
		int GetPostProcessTexture() const { return _framebuffer->GetColorAttachment(); }
		const TAGE::MEM::Ref<Framebuffer>& PostProcess::GetFramebuffer() const { return _framebuffer; }

		void SetSettings(const PostProcessSettings& settings) { _settings = settings; }
		PostProcessSettings& GetSettings() { return _settings; }

		void Resize(int newWidth, int newHeight) {
			if (newWidth != width || newHeight != height) {
				width = newWidth;
				height = newHeight;
				_framebuffer->Resize(newWidth, newHeight);
			}
		}
	private:
		TAGE::MEM::Ref<Framebuffer> _framebuffer;
		TAGE::MEM::Ref<Shader> _postProcessShader;
		PostProcessSettings _settings;
		int width = 0, height = 0;
	};
}
