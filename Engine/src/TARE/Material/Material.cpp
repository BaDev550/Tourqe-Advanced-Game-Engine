#include "tagepch.h"
#include "Material.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TAGE/AssetManager/AssetManager.h"
#include "TAGE/Project/Project.h"

namespace TARE {
	Material::Material(const char* name)
		: _Name(name)
	{
	}

	void Material::Use(TAGE::MEM::Ref<Shader>& shader)
	{
		shader->Use();
		 
		for (uint8 i = 0; i < (uint8)TextureType::COUNT; ++i) {
			TAGE::AssetHandle& handle = GetTexture((TextureType)i);
			if (handle == 0)
				continue;

			auto& texture = TAGE::AssetManager::GetAsset<Texture2D>(handle);
			if (texture)
				texture->Bind(i);
		}

		bool hasDiffuseTex = _TextureMaps.Diffuse != 0;
		bool hasAmbientOcclusionTex = _TextureMaps.AmbientOcclusion != 0;
		shader->SetUniform("u_Material.HasDiffuseTex", hasDiffuseTex);
		shader->SetUniform("u_Material.HasAmbientOcclusionTex", hasAmbientOcclusionTex);
		shader->SetUniform("u_Material.DiffuseTex",          (uint8)TextureType::DIFFUSE);
		shader->SetUniform("u_Material.DiffuseColor",        _Colors.Diffuse);
		shader->SetUniform("u_Material.SpecularTex",         (uint8)TextureType::SPECULAR);
		shader->SetUniform("u_Material.NormalTex",           (uint8)TextureType::NORMAL);
		shader->SetUniform("u_Material.RoughnessTex",        (uint8)TextureType::ROUGHNESS);
		shader->SetUniform("u_Material.MetallicTex",         (uint8)TextureType::METALLIC);
		shader->SetUniform("u_Material.AmbientOcclusionTex", (uint8)TextureType::AMBIENT_OCCLUSION);
	}

	void Material::SetTexture(TextureType type, TAGE::AssetHandle texture)
	{
		switch (type)
		{
		case TextureType::DIFFUSE:   _TextureMaps.Diffuse = texture; break;
		case TextureType::SPECULAR:  _TextureMaps.Specular = texture; break;
		case TextureType::NORMAL:    _TextureMaps.Normal = texture; break;
		case TextureType::ROUGHNESS: _TextureMaps.Roughness = texture; break;
		case TextureType::METALLIC:  _TextureMaps.Metallic = texture; break;
		case TextureType::AMBIENT_OCCLUSION: _TextureMaps.AmbientOcclusion = texture; break;
		default:
			break;
		}
	}

	TAGE::AssetHandle Material::GetTexture(TextureType type) const
	{
		switch (type)
		{
		case TextureType::DIFFUSE:   return _TextureMaps.Diffuse;
		case TextureType::SPECULAR:  return _TextureMaps.Specular;
		case TextureType::NORMAL:    return _TextureMaps.Normal;
		case TextureType::ROUGHNESS: return _TextureMaps.Roughness;
		case TextureType::METALLIC:  return _TextureMaps.Metallic;
		case TextureType::AMBIENT_OCCLUSION: return _TextureMaps.AmbientOcclusion;
		default:
			break;
		}
	}

	void Material::SetColor(TextureType slot, const glm::vec4& color) {
		switch (slot)
		{
		case TextureType::DIFFUSE: _Colors.Diffuse = color; break;
		default:
			break;
		}
	}

	glm::vec4 Material::GetColor(TextureType slot) const {
		switch (slot)
		{
		case TextureType::DIFFUSE:   return _Colors.Diffuse;
		default:
			break;
		}
	}
}