#pragma once
#include "Project.h"

namespace TAGE {
	class ProjectSerializer
	{
	public:
		ProjectSerializer(MEM::Ref<Project> project);

		bool Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

	private:
		MEM::Ref<Project> _Project;
	};
}