#include "tagepch.h"
#include "Material.h"

#include "TAGE/AssetManager/AssetManager.h"

namespace TARE {
	constexpr char* MATERIAL_DIFFUSE_MAP_NAME = "u_DiffuseMap";
	constexpr char* MATERIAL_SPECULAR_MAP_NAME = "u_SpecularMap";
	constexpr char* MATERIAL_NORMAL_MAP_NAME = "u_NormalMap";
	constexpr char* MATERIAL_METALLIC_MAP_NAME = "u_MetallicMap";
	constexpr char* MATERIAL_ROUGHNESS_MAP_NAME = "u_RoughnessMap";

	Material::Material(const TAGE::MEM::Ref<Shader>& shader)
		: _Shader(shader)
	{
	}

	TAGE::MEM::Ref<Material> Material::Create() {
		TAGE::MEM::Ref<Material> mat = TAGE::MEM::MakeRef<Material>(TARE3D::GetShaderLibrary().GetShader("DeferredGBuffer"));
		return std::move(mat);
	}

	void Material::BindTexture(TextureType type, uint8 slot)
	{
		if (HasTexture(type)) {
			switch (type)
			{
			case TARE::TextureType::Diffuse: {
				GetDiffuseTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_DIFFUSE_MAP_NAME, slot);
				break;
			}
			case TARE::TextureType::Specular: {
				GetSpecularTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_SPECULAR_MAP_NAME, slot);
				break;
			}
			case TARE::TextureType::Roughness: {
				GetRoughnessTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_ROUGHNESS_MAP_NAME, slot);
				break;
			}
			case TARE::TextureType::Normal: {
				GetNormalTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_NORMAL_MAP_NAME, slot);
				break;
			}
			case TARE::TextureType::Metallic: {
				GetMetalnessTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_METALLIC_MAP_NAME, slot);
				break;
			}
			case TARE::TextureType::AmbientOcclusion: {
				break;
			}
			}
		}
	}

	void Material::Use()
	{
		_Shader->Use();

		BindTexture(TextureType::Diffuse,   0);
		BindTexture(TextureType::Normal,    1);
		BindTexture(TextureType::Specular,  2);
		BindTexture(TextureType::Metallic,  3);
		BindTexture(TextureType::Roughness, 4);
	}

	void Material::SetTexture(TextureType type, TAGE::AssetHandle handle)
	{
		switch (type)
		{
		case TextureType::Diffuse:   SetDiffuseTexture(handle); break;
		case TextureType::Specular:  SetSpecularTexture(handle); break;
		case TextureType::Roughness: SetRoughnessTexture(handle); break;
		case TextureType::Normal:    SetNormalTexture(handle); break;
		case TextureType::Metallic:  SetMetalnessTexture(handle); break;
		default:
			break;
		}
	}

	TAGE::MEM::Ref<Texture2D> Material::GetDiffuseTexture() {  return TAGE::AssetManager::GetAsset<Texture2D>(_Maps.DiffuseMap); }
	TAGE::MEM::Ref<Texture2D> Material::GetNormalTexture() {   return TAGE::AssetManager::GetAsset<Texture2D>(_Maps.NormalMap);}
	TAGE::MEM::Ref<Texture2D> Material::GetSpecularTexture() { return TAGE::AssetManager::GetAsset<Texture2D>(_Maps.SpecularMap);}
	TAGE::MEM::Ref<Texture2D> Material::GetMetalnessTexture(){ return TAGE::AssetManager::GetAsset<Texture2D>(_Maps.MetalnessMap);}
	TAGE::MEM::Ref<Texture2D> Material::GetRoughnessTexture() {return TAGE::AssetManager::GetAsset<Texture2D>(_Maps.RoughnessMap);}

	void Material::SetDiffuseTexture(TAGE::AssetHandle texture)
	{
		_Maps.DiffuseMap = texture;
	}
	void Material::SetNormalTexture(TAGE::AssetHandle texture)
	{
		_Maps.NormalMap = texture;
	}
	void Material::SetSpecularTexture(TAGE::AssetHandle texture)
	{
		_Maps.SpecularMap = texture;
	}
	void Material::SetRoughnessTexture(TAGE::AssetHandle texture)
	{
		_Maps.RoughnessMap = texture;
	}
	void Material::SetMetalnessTexture(TAGE::AssetHandle texture)
	{
		_Maps.MetalnessMap = texture;
	}

	bool Material::HasTexture(TextureType type) const
	{
		switch (type)
		{
		case TextureType::Diffuse:   return _Maps.DiffuseMap != 0;
		case TextureType::Specular:  return _Maps.SpecularMap != 0;
		case TextureType::Roughness: return _Maps.RoughnessMap != 0;
		case TextureType::Normal:    return _Maps.NormalMap != 0;
		case TextureType::Metallic:  return _Maps.MetalnessMap != 0;
		default:
			break;
		}
		return false;
	}

	void Material::ResetAll()
	{
		_Maps.DiffuseMap != 0;
		_Maps.SpecularMap != 0;
		_Maps.RoughnessMap != 0;
		_Maps.NormalMap != 0;
		_Maps.MetalnessMap != 0;
	}
}