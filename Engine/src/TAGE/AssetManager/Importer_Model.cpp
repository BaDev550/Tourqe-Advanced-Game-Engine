#include "tagepch.h"
//#include "AssetManager.h"
//#include "AssetSerializers/AssetSerializer.h"
//
//namespace TAGE {
//	extern std::unordered_map<UUID, MEM::Ref<Texture>> _Textures;
//	extern std::unordered_map<UUID, MEM::Ref<Model>> _Models;
//
//	MEM::Ref<Model> AssetManager::ImportModel(const std::string& path)
//	{
//		std::string modelPath = GetAssetOutputPath(path, "Models", ".model");
//		MEM::Ref<Model> model = MEM::MakeRef<Model>();
//		_LoadingState = LoadingState::LOADING_DISK;
//
//		if (ModelSerializer::Deserialize(*model, modelPath)) {
//			UUID id;
//			_Models[id] = model;
//			_LoadingState = LoadingState::LOADING_COMPLETE;
//			return model;
//		}
//
//		if (model->LoadFromFile(path)) {
//			ModelSerializer::Serialize(*model, modelPath);
//
//			UUID id;
//			_Models[id] = model;
//
//			for (uint i = 0; i < model->GetMeshCount(); i++)
//			{
//				auto material = model->GetMesh(i).GetMaterial();
//				std::string materialPath = GetAssetOutputPath(material->GetName(), "Materials", ".material");
//				SaveMaterial(material, materialPath);
//			}
//
//			_LoadingState = LoadingState::LOADING_COMPLETE;
//			return model;
//		}
//
//		return nullptr;
//	}
//
//	MEM::Ref<Model> AssetManager::GetModel(UUID id)
//	{
//		return MEM::Ref<Model>();
//	}
//}