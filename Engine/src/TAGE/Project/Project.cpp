#include "tagepch.h"
#include "Project.h"
#include "ProjectSerializer.h"
#include "ProjectSettingsSerializer.h"
#include "TAGE/Scripting/ScriptEngine.h"

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
		ProjectSettingsSerializer settingsSerializer(project);

		if (serializer.Deserialize(path)) {
			project->_ProjectDirectory = path.parent_path();
			project->_ProjectSavePath = path;
			_ActiveProject = std::move(project);

			settingsSerializer.DeserializeRenderSettings(GetConfigDirectory() / "project-render.pconfig");
			settingsSerializer.DeserializeProjectDefaults(GetConfigDirectory() / "project-defaults.pconfig");

			MEM::Ref<AssetManagerEditor> editorAssetManager = MEM::MakeRef<AssetManagerEditor>();
			_ActiveProject->_AssetManager = editorAssetManager;

			editorAssetManager->DeserializeAssetRegistry();
			Logger::createFileLogger();
			ScriptEngine::Init();

			return _ActiveProject;
		}
		 
		return nullptr;
	}

	bool Project::SaveActive()
	{
		ProjectSerializer serializer(_ActiveProject);
		ProjectSettingsSerializer settingsSerializer(_ActiveProject);

		if (serializer.Serialize(_ActiveProject->_ProjectSavePath)) {
			settingsSerializer.SerializeRenderSettings(GetConfigDirectory() / "project-render.pconfig");
			settingsSerializer.SerializeProjectDefaults(GetConfigDirectory() / "project-defaults.pconfig");
			return true;
		}
		return false;
	}
}