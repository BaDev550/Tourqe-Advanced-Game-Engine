#pragma once
#include "TAGE/AssetManager/Asset.h"
#include "TARE/Shader/Shader.h"
#include "TARE/Texture/Texture2D.h"

namespace TARE {
	struct TextureMaps {
		TAGE::AssetHandle DiffuseMap = 0;
		TAGE::AssetHandle NormalMap = 0;
		TAGE::AssetHandle SpecularMap = 0;
		TAGE::AssetHandle RoughnessMap = 0;
		TAGE::AssetHandle MetalnessMap = 0;
	};

	class Material : public TAGE::Asset
	{
	public:
		Material() {}
		Material(const TAGE::MEM::Ref<Shader>& shader);

		void Use();
		void SetTexture(TextureType type, TAGE::AssetHandle handle);

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

		bool HasTexture(TextureType type) const;
		void ResetAll();

		TextureMaps GetTextures() const { return _Maps; }
		void SetTextures(TextureMaps textures) { _Maps = textures; }

		static TAGE::MEM::Ref<Material> Create();

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::Material; }
		virtual TAGE::AssetType GetType() const { return GetStaticType(); };
	private:
		TextureMaps _Maps;
		TAGE::MEM::Ref<Shader> _Shader = nullptr;
	};
}
