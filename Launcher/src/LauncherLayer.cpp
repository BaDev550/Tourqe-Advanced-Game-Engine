#include "LauncherLayer.h"
#include "TAGE/Project/ProjectSerializer.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_internal.h"
#include "LauncherSaver.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TAGE/World/Scene/SceneSerializer.h"
#include "TAGE/Utilities/Platform.h"
#include "TAGE/AssetManager/Importers/TextureImporter.h"
#include "TAGE/Common/TMath.h"
#include "TAGE/Application/Application.h"

#if defined(_WIN32)
#include <direct.h>
#include <windows.h>
#define ChangeDir _chdir
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define ChangeDir chdir
#endif

namespace TAGE::Launcher {
	std::string s_EngineDir;
	enum class LauncherTab { Projects, Create };
	static LauncherTab s_CurrentTab = LauncherTab::Projects;

	LauncherLayer::LauncherLayer()
	{
	}

	void LaunchEditor(const std::filesystem::path& editorPath, const std::filesystem::path& projectPath) {
		std::filesystem::path editorDir = editorPath.parent_path();
		std::string projectFile = (projectPath / (projectPath.stem().string() + ".tproj")).string();
		std::string command = editorPath.string() + " " + projectFile;

#if defined(_WIN32)
		STARTUPINFOA si = { sizeof(STARTUPINFOA) };
		PROCESS_INFORMATION pi;

		if (CreateProcessA(
			nullptr,
			command.data(),
			nullptr, nullptr, FALSE,
			CREATE_NEW_CONSOLE,
			nullptr,
			editorDir.string().c_str(),
			&si, &pi
		)) {
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}

#elif defined(__APPLE__) || defined(__linux__)
		pid_t pid = fork();
		if (pid == 0) {
			chdir(editorDir.string().c_str());
			execl(editorPath.string().c_str(), editorPath.filename().c_str(), projectFile.c_str(), (char*)nullptr);
			_exit(EXIT_FAILURE);
		}
#endif
		Application::Get()->Shutdown();
	}

	void LauncherLayer::OnImGuiRender()
	{
		TARE::RenderCommand::Clear(COLOR);

		static bool initialized = false;
		if (!initialized) {
			LauncherSave::Load("launcher-save.yaml");
			s_EngineDir = std::getenv("TAGE_DIR");
			initialized = true;
		}

		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar(3);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f });
		ImGui::PopStyleVar();

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		static bool first_time = true;
		if (first_time)
		{
			first_time = false;
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

			ImGuiID dock_main_id = dockspace_id;
			ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, nullptr, &dock_main_id);
			ImGui::DockBuilderDockWindow("Sidebar", dock_left_id);
			ImGui::DockBuilderDockWindow("All Projects", dock_main_id);
			ImGui::DockBuilderDockWindow("Create Project", dock_main_id);

			ImGui::DockBuilderFinish(dockspace_id);
		}

		DrawSidebar();
		DrawMainPanel();

		ImGui::End();
	}

	void LauncherLayer::DrawSidebar() {
		ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (ImGui::Button("Projects", ImVec2(-1, 40)))
			s_CurrentTab = LauncherTab::Projects;
		if (ImGui::Button("Create New", ImVec2(-1, 40)))
			s_CurrentTab = LauncherTab::Create;

		ImGui::End();
	}

	void LauncherLayer::DrawMainPanel()
	{
		switch (s_CurrentTab) {
		case LauncherTab::Projects: DrawProjectsPanel(); break;
		case LauncherTab::Create: DrawCreatePanel(); break;
		}
	}

	void LauncherLayer::DrawProjectsPanel() {
		ImGui::Begin("All Projects", nullptr, ImGuiWindowFlags_NoMove);
		ImGui::Text("Recent Projects");

		const float cardWidth = 220.0f;
		const float cardHeight = 140.0f;
		int columns = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (cardWidth + 20.0f)));
		ImGui::Columns(columns, nullptr, false);

		for (const auto& proj : LauncherSave::GetProjects()) {
			ImGui::BeginChild(proj.Name.c_str(), ImVec2(cardWidth, cardHeight), true, ImGuiWindowFlags_NoScrollbar);
			ImGui::TextWrapped(proj.Name.c_str());
			ImGui::TextWrapped(proj.Path.string().c_str());

			if (ImGui::Button(("Open##" + proj.Name).c_str())) {
				LaunchEditor(s_EngineDir + "/Editor/Editor.exe", proj.Path);
			}
			ImGui::EndChild();
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::End();
	}

	void LauncherLayer::DrawCreatePanel() {
		ImGui::Begin("Create Project", nullptr, ImGuiWindowFlags_NoMove);

		static char projectNameBuf[256] = "NewProject";
		static std::string projectDir;

		ImGui::InputText("Project Name", projectNameBuf, sizeof(projectNameBuf));

		if (ImGui::Button("Select Location")) {
			projectDir = Platform::FileDialog::OpenDirectory();
		}
		ImGui::Text("Path: %s", projectDir.empty() ? "(none)" : projectDir.c_str());

		if (ImGui::Button("Create & Open") && !projectDir.empty()) {
			std::string projectName = std::string(projectNameBuf);
			if (ProjectSerializer::CreateProjectDirectories(projectName, projectDir)) {
				std::filesystem::path fullPath = projectDir + "/" + projectName;
				LauncherSave::AddProject(projectName, fullPath);
				LauncherSave::Save("launcher-save.yaml");
				LaunchEditor(s_EngineDir + "/Editor/Editor.exe", fullPath);
			}
		}

		ImGui::End();
	}

	void LauncherLayer::ShowTemplate(int id, const std::string& name, const std::string& description, const std::string& iconPath)
	{
		if (ImGui::Selectable(name.c_str())) {
			_SelectedTemplate = id;
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", description.c_str());
	}
}


