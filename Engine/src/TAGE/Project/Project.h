#pragma once

#include "TAGE/Utilities/Logger.h"

#include "TAGE/AssetManager/AssetManagerRuntime.h"
#include "TAGE/AssetManager/AssetManagerEditor.h"
#include <string>
#include <filesystem>

namespace TAGE {
	struct ProjectConfig {
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path CacheDirectory;
		std::filesystem::path AssetRegistryPath;
		std::filesystem::path ScriptPath;
	};

	class Project
	{
	public:
		Project() = default;

		static const std::filesystem::path& GetProjectDirectory() { 
			ASSERT_NOMSG(_ActiveProject);
			return _ActiveProject->_ProjectDirectory;
		}
		static std::filesystem::path GetCacheDirectory() {
			ASSERT_NOMSG(_ActiveProject);
			return GetProjectDirectory() / _ActiveProject->_Config.CacheDirectory;
		}
		static std::filesystem::path GetAssetDirectory() {
			ASSERT_NOMSG(_ActiveProject);
			return GetProjectDirectory() / _ActiveProject->_Config.AssetDirectory;
		}
		static std::filesystem::path GetAssetRegistryPath() {
			ASSERT_NOMSG(_ActiveProject);
			return GetCacheDirectory() / _ActiveProject->_Config.AssetRegistryPath;
		}
		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) {
			ASSERT_NOMSG(_ActiveProject);
			return GetAssetDirectory() / path;
		}
		static std::filesystem::path GetScriptPath() {
			ASSERT_NOMSG(_ActiveProject);
			return GetProjectDirectory() / _ActiveProject->_Config.ScriptPath;
		}

		ProjectConfig& GetConfig() { return _Config; }
		static MEM::Ref<Project> GetActive() { return _ActiveProject; }
		MEM::Ref<AssetManagerBase>    GetAssetManager() { return _AssetManager; }
		MEM::Ref<AssetManagerRuntime> GetRuntimeAssetManager() { return std::static_pointer_cast<AssetManagerRuntime>(_AssetManager); }
		MEM::Ref<AssetManagerEditor>  GetEditorAssetManager() {  return std::static_pointer_cast<AssetManagerEditor>(_AssetManager); }

		static MEM::Ref<Project> New();
		static MEM::Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig _Config;
		std::filesystem::path _ProjectDirectory;
		MEM::Ref<AssetManagerBase> _AssetManager;

		inline static MEM::Ref<Project> _ActiveProject;
	};
}