#pragma once

#include "TAGE/Utilities/Logger.h"
#include <string>
#include <filesystem>

namespace TAGE {
	struct ProjectConfig {
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
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
		static std::filesystem::path GetAssetDirectory() {
			ASSERT_NOMSG(_ActiveProject);
			return GetProjectDirectory() / _ActiveProject->_Config.AssetDirectory;
		}
		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) {
			ASSERT_NOMSG(_ActiveProject);
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return _Config; }
		static MEM::Ref<Project> GetActive() { return _ActiveProject; }
		static MEM::Ref<Project> New();
		static MEM::Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig _Config;
		std::filesystem::path _ProjectDirectory;

		inline static MEM::Ref<Project> _ActiveProject;
	};
}