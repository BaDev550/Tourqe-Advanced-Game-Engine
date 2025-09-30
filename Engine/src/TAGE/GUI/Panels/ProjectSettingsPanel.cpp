#include "tagepch.h"
#include "ProjectSettingsPanel.h"
#include "TAGE/Application/Application.h"
#include "TAGE/GUI/GUIUtils.h"
#include "imgui.h"

namespace TAGE {
    void ProjectSettingsPanel::OnImGuiRender()
    {
        if (!_IsOpen) return;

        ImGui::Begin("Project Settings", &_IsOpen, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowSize(ImVec2(700, 600));

        ImGui::BeginChild("##LeftNav", ImVec2(150, 0), true);
        if (ImGui::Selectable("Render", _CurrentSetting == Settings::Graphics))
            _CurrentSetting = Settings::Graphics;
        if (ImGui::Selectable("Startup Scene", _CurrentSetting == Settings::StartupScene))
            _CurrentSetting = Settings::StartupScene;
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##SettingsContent", ImVec2(0, 0), false);

        DrawSettings(_CurrentSetting);

        ImGui::EndChild();
        ImGui::End();
    }

    void ProjectSettingsPanel::DrawSettings(Settings setting)
    {
        switch (setting)
        {
        case Settings::Graphics:
            DrawGraphicsSettings();
            break;
        case Settings::StartupScene:
            DrawStartupSceneSettings();
            break;
        }
    }

    void ProjectSettingsPanel::DrawGraphicsSettings()
    {
        if (ImGui::CollapsingHeader("Screen Space Settings")) {
            DrawScreenSpaceSettings();
        }
    }

    void ProjectSettingsPanel::DrawScreenSpaceSettings()
    {

    }

    void ProjectSettingsPanel::DrawStartupSceneSettings()
    {
		auto& project = Project::GetActive();
        AssetMetadata startupProjectData = Project::GetActive()->GetEditorAssetManager()->GetMetadata(project->GetConfig().StartScene);

        AssetHandle currentIndex = 0;
        if (GUI::ComboBox("Scene Asset", currentIndex, AssetType::Scene, startupProjectData.Handle)) {
            project->_Config.StartScene = Project::GetActive()->GetEditorAssetManager()->GetMetadata(currentIndex).FilePath;
        }
    }
}