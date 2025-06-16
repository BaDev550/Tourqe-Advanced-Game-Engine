#pragma once
#include "TAGE/Common/TDefines.h"
#include "TAGE/Common/TEnums.h"
#include "TAGE/Common/TTypes.h"
#include "TAGE/Utilities/Memory.h"
#include "TAGE/Renderer/Texture/Texture.h"
#include "TAGE/Renderer/Shader/Shader.h"

#include <array>

namespace TAGE::Renderer
{
	class Material
	{
	public:
		Material(const char* name, const char* shaderName);

		void Use() const;

		void SetTexture(TextureType slot, MEM::Ref<Texture2D> texture);
		Texture2D* GetTexture(TextureType slot) const;

		void SetTexturePaths(const std::vector<std::pair<TextureType, std::string>>& paths);
		void LoadTexturesFromPaths();

		std::string GetName() const { return _Name; }
	private:
		std::string _Name;
		MEM::Ref<Shader> _Shader;
		std::array<MEM::Ref<Texture2D>, static_cast<size_t>(TextureType::COUNT)> _Textures;
		std::unordered_map<TextureType, std::string> _TexturePaths;
	};
}