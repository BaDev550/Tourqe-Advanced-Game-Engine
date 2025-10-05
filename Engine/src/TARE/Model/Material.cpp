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

	void Material::BindTexture(MaterialTextureType type, uint8 slot)
	{
		if (HasTexture(type)) {
			switch (type)
			{
			case TARE::MaterialTextureType::Diffuse: {
				GetDiffuseTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_DIFFUSE_MAP_NAME, slot);
				break;
			}
			case TARE::MaterialTextureType::Specular: {
				GetSpecularTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_SPECULAR_MAP_NAME, slot);
				break;
			}
			case TARE::MaterialTextureType::Roughness: {
				GetRoughnessTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_ROUGHNESS_MAP_NAME, slot);
				break;
			}
			case TARE::MaterialTextureType::Normal: {
				GetNormalTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_NORMAL_MAP_NAME, slot);
				break;
			}
			case TARE::MaterialTextureType::Metallic: {
				GetMetalnessTexture()->Bind(slot);
				_Shader->SetUniform(MATERIAL_METALLIC_MAP_NAME, slot);
				break;
			}
			case TARE::MaterialTextureType::AmbientOcclusion: {
				break;
			}
			}
		}
	}

	void Material::Use()
	{
		_Shader->Use();

		_Shader->SetUniform("u_HasMetallicMap",  HasTexture(MaterialTextureType::Metallic));
		_Shader->SetUniform("u_HasRoughnessMap", HasTexture(MaterialTextureType::Roughness));
		_Shader->SetUniform("u_HasSpecularMap",  HasTexture(MaterialTextureType::Specular));
		_Shader->SetUniform("u_HasDiffuseMap", HasTexture(MaterialTextureType::Diffuse));

		_Shader->SetUniform("u_Metallic",  _Scalars.Metallic);
		_Shader->SetUniform("u_Roughness", _Scalars.Roughness);
		_Shader->SetUniform("u_Specular",  _Scalars.Specular);
		_Shader->SetUniform("u_DiffuseColor", _Colors.DiffuseColor);

		BindTexture(MaterialTextureType::Diffuse,   0);
		BindTexture(MaterialTextureType::Normal,    1);
		BindTexture(MaterialTextureType::Specular,  2);
		BindTexture(MaterialTextureType::Metallic,  3);
		BindTexture(MaterialTextureType::Roughness, 4);
	}

	void Material::SetTexture(MaterialTextureType type, TAGE::AssetHandle handle)
	{
		switch (type)
		{
		case MaterialTextureType::Diffuse:   SetDiffuseTexture(handle); break;
		case MaterialTextureType::Specular:  SetSpecularTexture(handle); break;
		case MaterialTextureType::Roughness: SetRoughnessTexture(handle); break;
		case MaterialTextureType::Normal:    SetNormalTexture(handle); break;
		case MaterialTextureType::Metallic:  SetMetalnessTexture(handle); break;
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

	bool Material::HasTexture(MaterialTextureType type) const
	{
		switch (type)
		{
		case MaterialTextureType::Diffuse:   return _Maps.DiffuseMap != 0;
		case MaterialTextureType::Specular:  return _Maps.SpecularMap != 0;
		case MaterialTextureType::Roughness: return _Maps.RoughnessMap != 0;
		case MaterialTextureType::Normal:    return _Maps.NormalMap != 0;
		case MaterialTextureType::Metallic:  return _Maps.MetalnessMap != 0;
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