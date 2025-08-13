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
		_Color[(int)TextureType::DIFFUSE] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		for (size_t i = 0; i < _Textures.size(); i++) { _Textures[i] = 0; }
	}

	void Material::Use(TAGE::MEM::Ref<Shader>& shader)
	{
		shader->Use();

		for (uint i = 0; i < static_cast<uint>(TextureType::COUNT); ++i) {
			if (i == SHADOW_MAP_TEXTURE_SLOT) continue;

			const auto& texture = TAGE::AssetManager::GetAsset<Texture2D>(_Textures[i]);
 			if (texture)
				texture->Bind(i);
		}

		bool hasDiffuseTex = false;
		shader->SetUniform("u_Material.HasDiffuseTex", hasDiffuseTex);
		if (hasDiffuseTex) { shader->SetUniform("u_Material.DiffuseTex", static_cast<int>(TextureType::DIFFUSE)); }
		else { shader->SetUniform("u_Material.DiffuseColor", _Color[static_cast<int>(TextureType::DIFFUSE)]); }
		shader->SetUniform("u_Material.SpecularTex", static_cast<int>(TextureType::SPECULAR));
		shader->SetUniform("u_Material.NormalTex", static_cast<int>(TextureType::NORMAL));
		shader->SetUniform("u_Material.HeightTex", static_cast<int>(TextureType::HEIGHT));
		shader->SetUniform("u_Material.RoughnessTex", static_cast<int>(TextureType::ROUGHNESS));
		shader->SetUniform("u_Material.MetallicTex", static_cast<int>(TextureType::METALLIC));
		shader->SetUniform("u_Material.AmbientOcclusionTex", static_cast<int>(TextureType::AMBIENT_OCCLUSION));
	}

	void Material::SetTexture(TextureType type, TAGE::AssetHandle texture)
	{
		_Textures[(int)type] = texture;
	}

	TAGE::AssetHandle Material::GetTexture(TextureType type) const
	{
		return _Textures[(int)type];
	}

	void Material::SetColor(TextureType slot, const glm::vec4& color) {
		if (slot < TextureType::COUNT)
			_Color[static_cast<uint>(slot)] = color;
	}

	glm::vec4 Material::GetColor(TextureType slot) const {
		if (slot < TextureType::COUNT)
			return _Color[static_cast<uint>(slot)];
		return glm::vec4(1.0f);
	}
}