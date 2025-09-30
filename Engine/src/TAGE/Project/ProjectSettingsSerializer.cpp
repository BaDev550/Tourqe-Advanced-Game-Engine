#include "tagepch.h"
#include "TAGE/Application/Application.h"
#include "ProjectSettingsSerializer.h"

#include <yaml-cpp/yaml.h>

namespace TAGE {
	ProjectSettingsSerializer::ProjectSettingsSerializer(MEM::Ref<Project> project)
		: _Project(project)
	{
	}

	bool ProjectSettingsSerializer::SerializeRenderSettings(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "RenderSettings" << YAML::Value;
		out << YAML::BeginMap;

		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
		return true;
	}

	bool ProjectSettingsSerializer::DeserializeRenderSettings(const std::filesystem::path& path)
	{
		YAML::Node data;
		try {
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e) {
			LOG_ERROR("Failed to load project: {}", path.string());
		}

		auto settingsNode = data["RenderSettings"];
		if (!settingsNode)
			return false;

		auto screenSpaceNode = settingsNode["ScreenSpace"];
		if (!screenSpaceNode)
			return false;

		return true;
	}

	bool ProjectSettingsSerializer::SerializeProjectDefaults(const std::filesystem::path& path)
	{
		const auto& config = _Project->GetConfig();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectDefaults" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "StartupScene" << YAML::Value << config.StartScene.string();
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
		return true;
	}
	bool ProjectSettingsSerializer::DeserializeProjectDefaults(const std::filesystem::path& path)
	{
		auto& config = _Project->GetConfig();
		YAML::Node data;
		try {
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e) {
			LOG_ERROR("Failed to load project: {}", path.string());
		}

		auto settingsNode = data["ProjectDefaults"];
		if (!settingsNode)
			return false;

		config.StartScene = settingsNode["StartupScene"].as<std::string>();

		return false;
	}
}