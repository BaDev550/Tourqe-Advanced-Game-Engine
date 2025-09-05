#include "tagepch.h"
#include "ProjectSerializer.h"
#include "ProjectSettingsSerializer.h"
#include "TAGE/World/Scene/SceneSerializer.h"
#include <cstdlib>

#include <yaml-cpp/yaml.h>

namespace TAGE {
#if defined(_WIN32)
	std::string generator = "vs2022";
#elif defined(__APPLE__)
	std::string generator = "xcode4";
#else
	std::string generator = "gmake2";
#endif

	ProjectSerializer::ProjectSerializer(MEM::Ref<Project> project)
		: _Project(project)
	{
	}

	bool ProjectSerializer::CreateProjectDirectories(const std::string& name, const std::filesystem::path& path)
	{
		ProjectConfig config = ProjectConfig();

		config.Name = name;
		config.ScriptPath = "Binaries/" + name + ".dll";
		const char* envPath = std::getenv("TAGE_DIR");
		if (!envPath) {
			LOG_ERROR("Environment variable TAGE_DIR not found!");
			return false;
		}

		std::filesystem::path engineDir = envPath;
		std::filesystem::path premakePath = engineDir / "vendor" / "premake" / "premake5.exe";
		std::filesystem::path projectDir = path / name;
		std::filesystem::path projectFile = projectDir / (name + ".tproj");
		std::filesystem::path configDir = projectDir / config.ConfigDirectory;

		std::string startupScene = "defaultScene";
		MEM::Ref<Scene> scene = MEM::MakeRef<Scene>(startupScene);
		SceneSerializer scene_serializer(scene);

		std::string command = "cd /D \"" + projectDir.string() + "\" && \"" + premakePath.string() + "\" " + generator;
		std::filesystem::create_directories(projectDir);
		if (CreatePremakeFile(name, projectDir)) {
			SerializeWithConfig(projectFile, config);
			std::filesystem::create_directories(projectDir / config.CacheDirectory);
			std::filesystem::create_directories(projectDir / config.CacheDirectory / "Logs");
			std::filesystem::create_directories(projectDir / config.ConfigDirectory);
			std::filesystem::create_directories(projectDir / config.AssetDirectory);
			std::filesystem::create_directories(projectDir / "Scripts" / "src");
			
			std::ofstream(projectDir / config.CacheDirectory / config.AssetRegistryPath);
			scene_serializer.Serialize(projectDir / config.AssetDirectory / (startupScene + ".scene"));
			std::ofstream(configDir / "project-render.pconfig");

			std::ofstream defaultsConfig(configDir / "project-defaults.pconfig");
			defaultsConfig << "ProjectDefaults:\n  StartupScene: " + (startupScene + ".scene");
			defaultsConfig.close();

			std::system(command.c_str());
			return true;
		}
		else {
			LOG_ERROR("Failed to create premake file!");
			return false;
		}
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& path)
	{
		const auto& config = _Project->GetConfig();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << config.Name;
		out << YAML::Key << "CacheDirectory" << YAML::Value << config.CacheDirectory.string();
		out << YAML::Key << "ConfigDirectory" << YAML::Value << config.ConfigDirectory.string();
		out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
		out << YAML::Key << "AssetRegistryPath" << YAML::Value << config.AssetRegistryPath.string();
		out << YAML::Key << "ScriptModulePath" << YAML::Value << config.ScriptPath.string();
		out << YAML::EndMap;

		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::SerializeWithConfig(const std::filesystem::path& path, const ProjectConfig& config)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << config.Name;
		out << YAML::Key << "CacheDirectory" << YAML::Value << config.CacheDirectory.string();
		out << YAML::Key << "ConfigDirectory" << YAML::Value << config.ConfigDirectory.string();
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
		config.CacheDirectory = projectNode["CacheDirectory"].as<std::string>();
		config.ConfigDirectory = projectNode["ConfigDirectory"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.ScriptPath = projectNode["ScriptModulePath"].as<std::string>();
		if (projectNode["AssetRegistryPath"])
			config.AssetRegistryPath = projectNode["AssetRegistryPath"].as<std::string>();
		return true;
	}

	bool ProjectSerializer::CreatePremakeFile(const std::string& projectName, const std::filesystem::path& projectDir)
	{
		const char* env = std::getenv("TAGE_DIR");
		if (!env) {
			LOG_ERROR("TAGE_DIR environment variable is not set!");
			return false;
		}
		std::filesystem::path engineDir = env;
		std::ofstream premake(projectDir / "premake5.lua");
		premake << "local TAGERootDir = \"" << engineDir.generic_string() << "/\"\n";
		premake << "workspace \"" << projectName << "\"\n";
		premake << "    architecture \"x86_64\"\n";
		premake << "    startproject \"" << projectName << "\"\n";
		premake << "\n";
		premake << "    configurations { \"Debug\", \"Release\" }\n";
		premake << "    flags { \"MultiProcessorCompile\" }\n";
		premake << "    outputdir = \"%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}\"\n";
		premake << "\n";
		premake << "project \"" << projectName << "\"\n";
		premake << "    kind \"SharedLib\"\n";
		premake << "    language \"C#\"\n";
		premake << "    dotnetframework \"4.7.2\"\n";
		premake << "    targetdir (\"Binaries\")\n";
		premake << "    objdir (\"Intermediates\")\n";
		premake << "    links { \"ScriptCore\" }\n";
		premake << "    files { \"Scripts/src/**.cs\", \"Scripts/properties/**.cs\" }\n";
		premake << "\n";
		premake << "    filter \"configurations:Debug\"\n";
		premake << "        runtime \"Debug\"\n";
		premake << "        optimize \"Off\"\n";
		premake << "        symbols \"Default\"\n";
		premake << "\n";
		premake << "    filter \"configurations:Release\"\n";
		premake << "        runtime \"Release\"\n";
		premake << "        optimize \"On\"\n";
		premake << "\n";
		premake << "group \"TAGE\"\n";
		premake << "    include (TAGERootDir .. \"ScriptCore/\")\n";
		premake << "group \"\"\n";
		return true;
	}
}