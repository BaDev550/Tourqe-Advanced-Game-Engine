#include "tagepch.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace TAGE {
	MEM::Ref<Project> Project::New()
	{
		_ActiveProject = MEM::MakeRef<Project>();
		return _ActiveProject;
	}

	MEM::Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		MEM::Ref<Project> project = MEM::MakeRef<Project>();
		ProjectSerializer serializer(project);

		if (serializer.Deserialize(path)) {
			project->_ProjectDirectory = path.parent_path();
			_ActiveProject = std::move(project);
			_ActiveProject->_AssetManager = MEM::MakeRef<AssetManagerEditor>();
			return _ActiveProject;
		}
		 
		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(_ActiveProject);
		if (serializer.Serialize(path)) {
			_ActiveProject->_ProjectDirectory = path.parent_path();
			return true;
		}
		return false;
	}
}