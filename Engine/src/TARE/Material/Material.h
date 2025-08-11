#pragma once
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Memory.h"
#include "TARE/Texture/Texture.h"
#include "TARE/Shader/Shader.h"

#include <array>

namespace TARE
{
	class Material
	{
	public:
		Material(const char* name);

		void Use(TAGE::MEM::Ref<Shader>& shader);

		void SetTexture(TextureType slot, TAGE::MEM::Ref<Texture2D> texture);
		Texture2D* GetTexture(TextureType slot) const;
		void SetColor(TextureType slot, const glm::vec4& color);
		glm::vec4 GetColor(TextureType slot) const;

		void SetTexturePaths(const std::vector<std::pair<TextureType, std::string>>& paths);

		std::string GetName() const { return _Name; }
	private:
		std::string _Name;
		std::array<TAGE::MEM::Ref<Texture2D>, static_cast<size_t>(TextureType::COUNT)> _Textures;
		std::array<glm::vec4, static_cast<size_t>(TextureType::COUNT)> _Color;
		std::unordered_map<TextureType, std::string> _TexturePaths;
	};
}