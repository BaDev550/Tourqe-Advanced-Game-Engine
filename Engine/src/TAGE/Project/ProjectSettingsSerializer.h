#pragma once
#include "Project.h"

namespace TAGE {
	class ProjectSettingsSerializer
	{
	public:
		ProjectSettingsSerializer(MEM::Ref<Project> project);

		bool SerializeRenderSettings(const std::filesystem::path& path);
		bool DeserializeRenderSettings(const std::filesystem::path& path);

		bool SerializeProjectDefaults(const std::filesystem::path& path);
		bool DeserializeProjectDefaults(const std::filesystem::path& path);
	private:
		MEM::Ref<Project> _Project;
	};
}

