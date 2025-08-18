#include "tagepch.h"
#include "ProjectSerializer.h"

#include <yaml-cpp/yaml.h>

namespace TAGE {
	ProjectSerializer::ProjectSerializer(MEM::Ref<Project> project)
		: _Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& path)
	{
		const auto& config = _Project->GetConfig();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << config.Name;
		out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.string();
		out << YAML::Key << "CacheDirectory" << YAML::Value << config.CacheDirectory.string();
		out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
		out << YAML::Key << "AssetRegistryPath" << YAML::Value << config.AssetRegistryPath.string();
		out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptPath.string();
		out << YAML::EndMap;

		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();

		return true;
	}
	bool ProjectSerializer::Deserialize(const std::filesystem::path& path)
	{
		auto& config = _Project->GetConfig();
		YAML::Node data;
		try {
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e) {
			LOG_ERROR("Failed to load project: {}", path.string());
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		config.StartScene = projectNode["StartScene"].as<std::string>();
		config.CacheDirectory = projectNode["CacheDirectory"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.ScriptPath = projectNode["ScriptModulePath"].as<std::string>();
		if (projectNode["AssetRegistryPath"])
			config.AssetRegistryPath = projectNode["AssetRegistryPath"].as<std::string>();
		return true;
	}
}