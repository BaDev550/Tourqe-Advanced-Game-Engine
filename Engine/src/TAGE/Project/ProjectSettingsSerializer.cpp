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
		const auto& config = Application::Get()->GetRenderer()->GetSceneData();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "RenderSettings" << YAML::Value;
		out << YAML::BeginMap;

		out << YAML::Key << "ScreenSpace" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "NumSamples" << YAML::Value << config.ScreenSpaceData.SS_NumSamples;
		out << YAML::Key << "SampleRadius" << YAML::Value << config.ScreenSpaceData.SS_SampleRadius;
		out << YAML::Key << "MaxSteps" << YAML::Value << config.ScreenSpaceData.SS_MaxSteps;
		out << YAML::Key << "StepSize" << YAML::Value << config.ScreenSpaceData.SS_StepSize;
		out << YAML::Key << "Tolerance" << YAML::Value << config.ScreenSpaceData.SS_Tolerance;
		out << YAML::Key << "Distance" << YAML::Value << config.ScreenSpaceData.SS_Distance;
		out << YAML::Key << "FadeStart" << YAML::Value << config.ScreenSpaceData.SS_FadeStart;
		out << YAML::Key << "FadeEnd" << YAML::Value << config.ScreenSpaceData.SS_FadeEnd;
		out << YAML::Key << "BinarySearchSteps" << YAML::Value << config.ScreenSpaceData.SS_BinarySearchSteps;
		out << YAML::Key << "Thickness" << YAML::Value << config.ScreenSpaceData.SS_Thickness;
		out << YAML::Key << "Intensity" << YAML::Value << config.ScreenSpaceData.SS_Intensity;
		out << YAML::Key << "MaxReflectionLOD" << YAML::Value << config.ScreenSpaceData.SS_MaxReflectionLOD;
		out << YAML::Key << "GlobalIlluminationIntensity" << YAML::Value << config.ScreenSpaceData.SS_GlobalIlluminationIntensity;
		out << YAML::Key << "UseSSGI" << YAML::Value << config.UseSSGI;
		out << YAML::Key << "UseSSR" << YAML::Value << config.UseSSReflections;
		out << YAML::EndMap; // End ScreenSpace

		out << YAML::EndMap;

		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
		return true;
	}

	bool ProjectSettingsSerializer::DeserializeRenderSettings(const std::filesystem::path& path)
	{
		auto& config = Application::Get()->GetRenderer()->GetSceneData();
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

		config.ScreenSpaceData.SS_NumSamples = screenSpaceNode["NumSamples"].as<int>(config.ScreenSpaceData.SS_NumSamples);
		config.ScreenSpaceData.SS_SampleRadius = screenSpaceNode["SampleRadius"].as<float>(config.ScreenSpaceData.SS_SampleRadius);
		config.ScreenSpaceData.SS_MaxSteps = screenSpaceNode["MaxSteps"].as<int>(config.ScreenSpaceData.SS_MaxSteps);
		config.ScreenSpaceData.SS_StepSize = screenSpaceNode["StepSize"].as<int>(config.ScreenSpaceData.SS_StepSize);
		config.ScreenSpaceData.SS_Tolerance = screenSpaceNode["Tolerance"].as<int>(config.ScreenSpaceData.SS_Tolerance);
		config.ScreenSpaceData.SS_Distance = screenSpaceNode["Distance"].as<int>(config.ScreenSpaceData.SS_Distance);
		config.ScreenSpaceData.SS_FadeStart = screenSpaceNode["FadeStart"].as<float>(config.ScreenSpaceData.SS_FadeStart);
		config.ScreenSpaceData.SS_FadeEnd = screenSpaceNode["FadeEnd"].as<float>(config.ScreenSpaceData.SS_FadeEnd);
		config.ScreenSpaceData.SS_BinarySearchSteps = screenSpaceNode["BinarySearchSteps"].as<int>(config.ScreenSpaceData.SS_BinarySearchSteps);
		config.ScreenSpaceData.SS_Thickness = screenSpaceNode["Thickness"].as<float>(config.ScreenSpaceData.SS_Thickness);
		config.ScreenSpaceData.SS_Intensity = screenSpaceNode["Intensity"].as<float>(config.ScreenSpaceData.SS_Intensity);
		config.ScreenSpaceData.SS_MaxReflectionLOD = screenSpaceNode["MaxReflectionLOD"].as<float>(config.ScreenSpaceData.SS_MaxReflectionLOD);
		config.ScreenSpaceData.SS_GlobalIlluminationIntensity = screenSpaceNode["GlobalIlluminationIntensity"].as<float>(config.ScreenSpaceData.SS_GlobalIlluminationIntensity);
		config.UseSSGI = screenSpaceNode["UseSSGI"].as<bool>(config.UseSSGI);
		config.UseSSReflections = screenSpaceNode["UseSSR"].as<bool>(config.UseSSReflections);

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
		auto& config = _Project->_Config;
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