#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace TAGE::Launcher {
    struct SavedProject {
        std::string Name;
        std::filesystem::path Path;
    };

    class LauncherSave {
    public:
        static bool Load(const std::filesystem::path& file);
        static bool Save(const std::filesystem::path& file);
        static void AddProject(const std::string& name, const std::filesystem::path& path);
        static const std::vector<SavedProject>& GetProjects();

    private:
        static std::vector<SavedProject> s_Projects;
    };
}