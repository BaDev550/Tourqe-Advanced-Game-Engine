#pragma once
#include "TAGE/AssetManager/Asset.h"
#include "TARE/Shader/Shader.h"
#include "TARE/Texture/Texture2D.h"

namespace TARE {
	enum class MaterialTextureType {
		Diffuse,
		Specular,
		Roughness,
		Normal,
		Metallic,
		AmbientOcclusion
	};

	struct TextureMaps {
		TAGE::AssetHandle DiffuseMap = 0;
		TAGE::AssetHandle NormalMap = 0;
		TAGE::AssetHandle SpecularMap = 0;
		TAGE::AssetHandle RoughnessMap = 0;
		TAGE::AssetHandle MetalnessMap = 0;
	};
	struct ScalarMaps {
		float Metallic = 0.0f;
		float Roughness = 0.0f;
		float Specular = 0.5f;
	};
	struct ColorMaps {
		glm::vec3 DiffuseColor = { 1.0f, 1.0f, 1.0f };
		glm::vec3 SpecularColor = { 1.0f, 1.0f, 1.0f };
	};

	class Material : public TAGE::Asset
	{
	public:
		Material() {}
		Material(const TAGE::MEM::Ref<Shader>& shader);

		void Use();
		void SetTexture(MaterialTextureType type, TAGE::AssetHandle handle);

		TAGE::MEM::Ref<Texture2D> GetDiffuseTexture();
		void SetDiffuseTexture(TAGE::AssetHandle texture);

		TAGE::MEM::Ref<Texture2D> GetNormalTexture();
		void SetNormalTexture(TAGE::AssetHandle texture);

		TAGE::MEM::Ref<Texture2D> GetSpecularTexture();
		void SetSpecularTexture(TAGE::AssetHandle texture);

		TAGE::MEM::Ref<Texture2D> GetRoughnessTexture();
		void SetRoughnessTexture(TAGE::AssetHandle texture);

		TAGE::MEM::Ref<Texture2D> GetMetalnessTexture();
		void SetMetalnessTexture(TAGE::AssetHandle texture);

		void SetMetallic(float metallic) { _Scalars.Metallic = metallic; }
		void SetRoughness(float roughness) { _Scalars.Roughness = roughness; }
		void SetSpecular(float specular) { _Scalars.Specular = specular; }

		void SetDiffuseColor(const glm::vec3& color) { _Colors.DiffuseColor = color; }
		void SetSpecularColor(const glm::vec3& color) { _Colors.SpecularColor = color; }

		bool HasTexture(MaterialTextureType type) const;
		void ResetAll();

		TextureMaps GetTextures() const { return _Maps; }
		ScalarMaps GetScalars() const { return _Scalars; }
		ColorMaps GetColors() const { return _Colors; }
		void SetTextures(TextureMaps textures) { _Maps = textures; }
		void SetScalars(ScalarMaps scalars) { _Scalars = scalars; }
		void SetColors(ColorMaps colors) { _Colors = colors; }

		static TAGE::MEM::Ref<Material> Create();

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::Material; }
		virtual TAGE::AssetType GetType() const { return GetStaticType(); };
	private:
		TextureMaps _Maps;
		ScalarMaps _Scalars;
		ColorMaps _Colors;
		TAGE::MEM::Ref<Shader> _Shader = nullptr;

		void BindTexture(MaterialTextureType type, uint8 slot);
	};
}
