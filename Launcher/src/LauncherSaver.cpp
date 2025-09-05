#include "LauncherSaver.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace TAGE::Launcher {
    std::vector<SavedProject> LauncherSave::s_Projects;

    bool LauncherSave::Load(const std::filesystem::path& file) {
        if (!std::filesystem::exists(file))
            return false;

        YAML::Node data = YAML::LoadFile(file.string());
        if (!data["Projects"])
            return false;

        s_Projects.clear();
        for (const auto& project : data["Projects"]) {
            SavedProject p;
            p.Name = project["Name"].as<std::string>();
            p.Path = project["Path"].as<std::string>();
            s_Projects.push_back(p);
        }

        return true;
    }

    bool LauncherSave::Save(const std::filesystem::path& file) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Projects" << YAML::Value << YAML::BeginSeq;
        for (const auto& p : s_Projects) {
            out << YAML::BeginMap;
            out << YAML::Key << "Name" << YAML::Value << p.Name;
            out << YAML::Key << "Path" << YAML::Value << p.Path.string();
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(file);
        fout << out.c_str();
        return true;
    }

    void LauncherSave::AddProject(const std::string& name, const std::filesystem::path& path) {
        s_Projects.push_back({ name, path });
    }

    const std::vector<SavedProject>& LauncherSave::GetProjects() {
        return s_Projects;
    }
}