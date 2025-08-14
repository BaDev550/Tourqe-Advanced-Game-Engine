#include "tagepch.h"
#include "Material.h"
#include "TARE/Shader/ShaderLibrary.h"
#include "TAGE/AssetManager/AssetManager.h"
#include "TAGE/Project/Project.h"

namespace TARE {
	Material::Material(const char* name)
		: _Name(name)
		, _Textures{}
		, _Color{}
	{
		_Color[(uint8)TextureType::DIFFUSE] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		for (size_t i = 0; i < _Textures.size(); i++) { _Textures[i] = 0; }
	}

	void Material::Use(TAGE::MEM::Ref<Shader>& shader)
	{
		shader->Use();
		 
		for (uint8 i = 0; i < (uint8)TextureType::COUNT; ++i) {
			if (i == SHADOW_MAP_TEXTURE_SLOT) continue;

			if (_Textures[i] != 0) {
				const auto& texture = TAGE::AssetManager::GetAsset<Texture2D>(_Textures[i]);
				if (texture)
					texture->Bind(i);
			}
		}

		bool hasDiffuseTex = true;
		shader->SetUniform("u_Material.HasDiffuseTex", hasDiffuseTex);
		shader->SetUniform("u_Material.DiffuseTex",          static_cast<uint8>(TextureType::DIFFUSE));
		shader->SetUniform("u_Material.SpecularTex",         static_cast<uint8>(TextureType::SPECULAR));
		shader->SetUniform("u_Material.NormalTex",           static_cast<uint8>(TextureType::NORMAL));
		shader->SetUniform("u_Material.HeightTex",           static_cast<uint8>(TextureType::HEIGHT));
		shader->SetUniform("u_Material.RoughnessTex",        static_cast<uint8>(TextureType::ROUGHNESS));
		shader->SetUniform("u_Material.MetallicTex",         static_cast<uint8>(TextureType::METALLIC));
		shader->SetUniform("u_Material.AmbientOcclusionTex", static_cast<uint8>(TextureType::AMBIENT_OCCLUSION));
	}

	void Material::SetTexture(TextureType type, TAGE::AssetHandle texture)
	{
		_Textures[(uint8)type] = texture;
	}

	TAGE::AssetHandle Material::GetTexture(TextureType type) const
	{
		return _Textures[(uint8)type];
	}

	void Material::SetColor(TextureType slot, const glm::vec4& color) {
		if (slot < TextureType::COUNT)
			_Color[static_cast<uint8>(slot)] = color;
	}

	glm::vec4 Material::GetColor(TextureType slot) const {
		if (slot < TextureType::COUNT)
			return _Color[static_cast<uint8>(slot)];
		return glm::vec4(1.0f);
	}
}