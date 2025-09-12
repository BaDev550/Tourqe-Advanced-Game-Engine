#include "tagepch.h"
#include "ProjectSettingsPanel.h"
#include "TAGE/Application/Application.h"
#include "TARE/TARE.h"
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
		auto& ssrData = TAGE::Application::Get()->GetRenderer()->GetSceneData().ScreenSpaceData;
		ImGui::SliderInt("SS Num Samples", &ssrData.SS_NumSamples, 1, 64);
		ImGui::SliderFloat("SS Sample Radius", &ssrData.SS_SampleRadius, 0.1f, 10.0f);
		ImGui::SliderInt("SS Max Steps", &ssrData.SS_MaxSteps, 1, 64);
		ImGui::SliderInt("SS Step Size", &ssrData.SS_StepSize, 1, 64);
		ImGui::SliderInt("SS Tolerance", &ssrData.SS_Tolerance, 1, 64);
		ImGui::SliderInt("SS Distance", &ssrData.SS_Distance, 1, 64);
		ImGui::SliderFloat("SS Fade Start", &ssrData.SS_FadeStart, 0.0f, 1.0f);
		ImGui::SliderFloat("SS Fade End", &ssrData.SS_FadeEnd, 0.0f, 1.0f);
		ImGui::SliderInt("SS Binary Search Steps", &ssrData.SS_BinarySearchSteps, 1, 10);
		ImGui::SliderFloat("SS Thickness", &ssrData.SS_Thickness, 0.0f, 10.0f);
		ImGui::SliderFloat("SS Intensity", &ssrData.SS_Intensity, 0.0f, 10.0f);
		ImGui::SliderFloat("SS Max Reflection LOD", &ssrData.SS_MaxReflectionLOD, 0.0f, 10.0f);
		ImGui::SliderFloat("SS Global Illumination Intensity", &ssrData.SS_GlobalIlluminationIntensity, 0.0f, 10.0f);
		ImGui::Checkbox("Use Screen Space Global Illumination", &TAGE::Application::Get()->GetRenderer()->GetSceneData().UseSSGI);
		ImGui::Checkbox("Use Screen Space Reflections", &TAGE::Application::Get()->GetRenderer()->GetSceneData().UseSSReflections);
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