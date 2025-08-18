#pragma once
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Memory.h"
#include "TARE/Texture/Texture.h"
#include "TARE/Shader/Shader.h"
#include "TAGE/AssetManager/Asset.h"

#include <array>

namespace TARE
{
	class Material : public TAGE::Asset
	{
	public:
		Material(const char* name);

		void Use(TAGE::MEM::Ref<Shader>& shader);

		void SetTexture(TextureType slot, TAGE::AssetHandle texture);
		TAGE::AssetHandle GetTexture(TextureType slot) const;

		void SetColor(TextureType slot, const glm::vec4& color);
		glm::vec4 GetColor(TextureType slot) const;
		std::string GetName() const { return _Name; }

		static TAGE::AssetType GetStaticType() { return TAGE::AssetType::Material; }
		virtual TAGE::AssetType GetType() const { return GetStaticType(); };
	private:
		std::string _Name;
		struct TextureMaps {
			TAGE::AssetHandle Diffuse = 0;
			TAGE::AssetHandle Specular = 0;
			TAGE::AssetHandle Normal = 0;
			TAGE::AssetHandle Roughness = 0;
			TAGE::AssetHandle Metallic = 0;
			TAGE::AssetHandle AmbientOcclusion = 0;
		} _TextureMaps;

		struct Colors {
			glm::vec4 Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		} _Colors;
	};
}