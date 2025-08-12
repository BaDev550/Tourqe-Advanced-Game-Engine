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
		std::array<TAGE::AssetHandle, static_cast<size_t>(TextureType::COUNT)> _Textures;
		std::array<glm::vec4, static_cast<size_t>(TextureType::COUNT)> _Color;
	};
}