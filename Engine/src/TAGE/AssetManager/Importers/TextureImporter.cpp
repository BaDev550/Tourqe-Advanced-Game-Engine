#include "tagepch.h"
#include "TextureImporter.h"
#include "TAGE/MultiThread/MultiThread.h"
#include "TAGE/Project/Project.h"

#define STB_DXT_IMPLEMENTATION
#include <stb/stb_dxt.h>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace TAGE {
	MEM::Ref<TARE::Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		PROFILE_FUNCTION();

		return LoadTexture2D(Project::GetAssetDirectory() / metadata.FilePath);
	}

	MEM::Ref<TARE::Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();

		int width, height, channels;
		Buffer data;
		{
			PROFILE_SCOPE("stbi_load - TextureImporter::ImportTexture2D");
			data.Data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);
		}

		if (data.Data != nullptr) {
			TARE::TextureSpecs textureSpecs;
			textureSpecs.Width = width;
			textureSpecs.Height = height;
			textureSpecs.Channels = channels;
			data.Size = width * height * channels;

			MEM::Ref<TARE::Texture2D> texture = TARE::Texture2D::Create(textureSpecs, data);
			data.Release();
			return texture;
		}
		else {
			LOG_ERROR("Failed to load texture from: ", filepath.string());
		}
	}
}