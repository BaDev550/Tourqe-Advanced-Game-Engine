#include "tagepch.h"
#include "Material.h"
#include "TAGE/Renderer/Shader/ShaderLibrary.h"
#include "TAGE/AssetManager/AssetManager.h"

namespace TAGE::Renderer {
	Material::Material(const char* name, const char* shaderName)
		: _Name(name), _Shader(ShaderLibrary::Get(shaderName))
		, _Textures{}
	{

	}

	void Material::Use() const
	{
		if (_Shader) {
			static MEM::Scope<Texture2D> fallbackTexture = MEM::MakeScope<Texture2D>("");

			for (uint i = 0; i < static_cast<uint>(TextureType::COUNT); ++i) {
				if (i == SHADOW_MAP_TEXTURE_SLOT) continue;

				const auto& texture = _Textures[i];
				if (texture)
					texture->Bind(i);
				else if (_Textures[0])
					_Textures[0]->Bind(i);
				else
					fallbackTexture->Bind(i);
			}

			_Shader->SetUniform("u_Material.DiffuseTex", static_cast<int>(TextureType::DIFFUSE));
			_Shader->SetUniform("u_Material.SpecularTex", static_cast<int>(TextureType::SPECULAR));
			_Shader->SetUniform("u_Material.NormalTex", static_cast<int>(TextureType::NORMAL));
			_Shader->SetUniform("u_Material.HeightTex", static_cast<int>(TextureType::HEIGHT));
			_Shader->SetUniform("u_Material.RoughnessTex", static_cast<int>(TextureType::ROUGHNESS));
			_Shader->SetUniform("u_Material.MetallicTex", static_cast<int>(TextureType::METALLIC));
			_Shader->SetUniform("u_Material.AmbientOcclusionTex", static_cast<int>(TextureType::AMBIENT_OCCLUSION));
		}
		else {
			LOG_ERROR("Material shader is not set or does not exist: {}", _Name);
		}
	}

	void Material::SetTexture(TextureType type, MEM::Ref<Texture2D> texture)
	{
		if (texture)
			_Textures[static_cast<uint>(type)] = texture;
	}

	Texture2D* Material::GetTexture(TextureType type) const
	{
		return _Textures[static_cast<uint>(type)].get();
	}

	void Material::SetTexturePaths(const std::vector<std::pair<TextureType, std::string>>& paths) {
		_TexturePaths.clear();
		for (auto& [slot, path] : paths)
			_TexturePaths[slot] = path;
	}

	void Material::LoadTexturesFromPaths() {
		for (auto& [slot, path] : _TexturePaths) {
			if (!path.empty()) {
				auto tex = AssetManager::ImportTexture(path);
				SetTexture(slot, tex);
			}
		}
	}
}