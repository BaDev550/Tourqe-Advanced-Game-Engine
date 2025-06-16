#include "tagepch.h"
//#include "AssetManager.h"
//#include "AssetSerializers/AssetSerializer.h"
//
//namespace TAGE {
//	extern std::unordered_map<UUID, MEM::Ref<Texture2D>> _Textures;
//	extern std::unordered_map<UUID, MEM::Ref<Model>> _Models;
//
//	MEM::Ref<Texture2D> AssetManager::ImportTexture(const std::string& path)
//	{
//		UUID id = UUID(path);
//		if (_Textures.find(id) != _Textures.end())
//			return _Textures[id];
//
//		auto texture = MEM::MakeRef<Texture2D>();
//		if (texture->LoadTexture(path)) {
//			_Textures[id] = texture;
//			return texture;
//		}
//		return nullptr;
//	}
//
//	MEM::Ref<Texture2D> AssetManager::ImportEmbeddedTexture(const std::string& name, const unsigned char* data, size_t size)
//	{
//		std::string texturePath = GetAssetOutputPath(name, "Textures", ".png");
//
//		UUID id = UUID();
//		if (_Textures.find(id) != _Textures.end())
//			return _Textures[id];
//
//		auto texture = MEM::MakeRef<Texture2D>();
//		if (texture->LoadFromMemory(data, size)) {
//			texture->SaveToDisk(texturePath);
//			_Textures[id] = texture;
//			return texture;
//		}
//		return nullptr;
//	}
//
//	MEM::Ref<Texture2D> AssetManager::ImportEmbeddedTexture(const std::string& name, const unsigned char* data, int width, int height, int channels)
//	{
//		std::string texturePath = GetAssetOutputPath(name, "Textures", ".png");
//
//		UUID id = UUID();
//		if (_Textures.find(id) != _Textures.end())
//			return _Textures[id];
//
//		auto texture = MEM::MakeRef<Texture2D>();
//		if (texture->LoadFromMemory(data, width, height, channels)) {
//			texture->SaveToDisk(texturePath);
//			_Textures[id] = texture;
//			return texture;
//		}
//		return nullptr;
//	}
//
//	MEM::Ref<Texture2D> AssetManager::GetTexture(UUID id)
//	{
//		return MEM::Ref<Texture2D>();
//	}
//
//	MEM::Ref<Material> AssetManager::ImportMaterial(const std::string& matPath)
//	{
//		MEM::Ref<Material> material = MEM::MakeRef<Material>("ImportedMaterial", "MainShader");
//		
//		if (!MaterialSerializer::Deserialize(*material, matPath)) {
//			LOG_WARN("Failed to deserialize material {}", matPath);
//			return material;
//		}
//
//		material->LoadTexturesFromPaths();
//		return material;
//	}
//
//	bool AssetManager::SaveMaterial(MEM::Ref<Material> material, const std::string& matPath)
//	{
//		return MaterialSerializer::Serialize(*material, matPath);
//	}
//
//	bool AssetManager::LoadMaterial(MEM::Ref<Material> material, const std::string& matPath)
//	{
//		return MaterialSerializer::Deserialize(*material, matPath);;
//	}
//}