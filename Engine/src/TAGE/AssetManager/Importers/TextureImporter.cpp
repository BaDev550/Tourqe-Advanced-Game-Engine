#include "tagepch.h"
#include "TextureImporter.h"
#include "TAGE/MultiThread/MultiThread.h"
#include "TAGE/Project/Project.h"

#define STB_DXT_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_dxt.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace TAGE {
	MEM::Ref<TARE::Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		PROFILE_FUNCTION();
		return LoadTexture2D(Project::GetAssetDirectory() / metadata.FilePath);
	}
	
	bool TextureImporter::WriteTextureTo(const std::filesystem::path& filepath, const std::filesystem::path& to)
	{
		PROFILE_FUNCTION();

		int width = 0, height = 0, channels = 0;
		Buffer data;
		if (!to.empty()) {
			data.Data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);
			PROFILE_SCOPE("stbi_load - TextureImporter::WriteTextureTo");
			if (channels == 4) {
				stbi_write_png(to.string().c_str(), width, height, channels, data.Data, width * channels);
			}
			else if (channels == 3) {
				stbi_write_jpg(to.string().c_str(), width, height, channels, data.Data, width * channels);
			}
		}

		if (data.Data != nullptr) {
			data.Release();
			return true;
		}
		else {
			LOG_ERROR("Failed to write texture to: ", to.string());
		}
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
			textureSpecs.format = TARE::TextureFormat::NONE;
			textureSpecs.InternalFormat = TARE::TextureFormat::NONE;
			data.Size = width * height * channels;

			MEM::Ref<TARE::Texture2D> texture = TARE::Texture2D::Create(textureSpecs, data);
			return texture;
		}
		else {
			LOG_ERROR("Failed to load texture from: ", filepath.string());
		}
	}

	MEM::Ref<TARE::EnviromentMap> EnviromentMapImporter::ImportEnviromentMap(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadEnviromentMap(Project::GetAssetDirectory() / metadata.FilePath);
	}

	MEM::Ref<TARE::EnviromentMap> EnviromentMapImporter::LoadEnviromentMap(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();

		int width = 0, height = 0, channels = 0;
		TAGE::Buffer data;
		if (stbi_is_hdr(filepath.string().c_str())) {
			{
				PROFILE_SCOPE("stbi_load - TextureImporter::ImportTexture2D");
				data.Data = stbi_loadf(filepath.string().c_str(), &width, &height, &channels, 0);
				data.Size = width * height * channels * sizeof(float);
			}
		}
		else {
			LOG_ERROR("Texture is not in HDR format: ", filepath.string());
		}

		if (data.Data != nullptr) {
			TARE::TextureSpecs textureSpecs;
			textureSpecs.Width = width;
			textureSpecs.Height = height;
			textureSpecs.Channels = channels;
			textureSpecs.CreateMipmaps = false;
			textureSpecs.DataType = TARE::TextureDataType::Float;
			textureSpecs.format = TARE::TextureFormat::RGB;
			textureSpecs.InternalFormat = TARE::TextureFormat::RGB16F;
			MEM::Ref<TARE::Texture2D> hdrTexture = TARE::Texture2D::Create(textureSpecs, data);
			MEM::Ref<TARE::TextureCube> envCubeMap = TARE::TARE3D::EquirectangularToCubemap(hdrTexture);
			MEM::Ref<TARE::TextureCube> irradianceMap = TARE::TARE3D::CreateIrradianceMap(envCubeMap);
			MEM::Ref<TARE::EnviromentMap> envMap = TAGE::MEM::MakeRef<TARE::EnviromentMap>(envCubeMap, irradianceMap, hdrTexture);
			return envMap;
		}
		else {
			LOG_ERROR("Failed to load texture from: ", filepath.string());
		}
	}
}