#pragma once
#include "Project.h"

namespace TAGE {
	class ProjectSerializer
	{
	public:
		ProjectSerializer(MEM::Ref<Project> project);

		bool Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

		static bool CreateProjectDirectories(const std::string& name, const std::filesystem::path& path);
	private:
		static bool SerializeWithConfig(const std::filesystem::path& path, const ProjectConfig& config);
		static bool CreatePremakeFile(const std::string& projectName, const std::filesystem::path& projectDir);
	private:
		MEM::Ref<Project> _Project;
	};
}