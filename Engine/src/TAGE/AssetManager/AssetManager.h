//#pragma once
//#include "TAGE/Common/TEnums.h"
//#include <unordered_map>
//
//#include "TAGE/Renderer/Texture/Texture.h"
//#include "TAGE/Renderer/Model/Model.h"
//#include "TAGE/Utilities/UUID.h"
//#include <filesystem>
//
//namespace TAGE {
//	using namespace Renderer;
//	class AssetManager {
//	public:
//		AssetManager();
//
//		static MEM::Ref<Texture2D> ImportTexture(const std::string& path);
//		static MEM::Ref<Texture2D> ImportEmbeddedTexture(const std::string& name, const unsigned char* data, size_t size);
//		static MEM::Ref<Texture2D> ImportEmbeddedTexture(const std::string& name, const unsigned char* data, int width, int height, int channels);
//
//		static MEM::Ref<Model> ImportModel(const std::string& path);
//		static MEM::Ref<Material> ImportMaterial(const std::string& matPath);
//		static bool SaveMaterial(MEM::Ref<Material> material, const std::string& matPath);
//		static bool LoadMaterial(MEM::Ref<Material> material, const std::string& matPath);
//
//		static MEM::Ref<Texture2D> GetTexture(UUID id);
//		static MEM::Ref<Model> GetModel(UUID id);
//
//		static std::string GetAssetOutputPath(const std::string& originalPath, const std::string& folder, const std::string& extension) {
//			std::filesystem::path orig(originalPath);
//			if (std::filesystem::exists(ASSET_CONTENT_PATH + folder))
//				return ASSET_CONTENT_PATH + folder + "/" + orig.stem().string() + extension;
//			else {
//				std::filesystem::create_directories(ASSET_CONTENT_PATH + folder);
//				return ASSET_CONTENT_PATH + folder + "/" + orig.stem().string() + extension;
//			}
//		}
//		static std::string GetAssetFolder(const std::string& originalPath, const std::string& folder) {
//			std::filesystem::path orig(originalPath);
//			return ASSET_CONTENT_PATH + folder;
//		}
//	private:
//		static std::unordered_map<UUID, MEM::Ref<Texture2D>> _Textures;
//		static std::unordered_map<UUID, MEM::Ref<Model>> _Models;
//		static inline LoadingState _LoadingState = LoadingState::AWAITING_DISK;
//	};
//}