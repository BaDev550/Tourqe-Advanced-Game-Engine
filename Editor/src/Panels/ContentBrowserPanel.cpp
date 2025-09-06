#include "ContentBrowserPanel.h"
#include "imgui.h"
#include "TAGE/World/Scene/SceneSerializer.h"
#include "TAGE/Project/Project.h"
#include "TAGE/AssetManager/Importers/TextureImporter.h"
#include "TAGE/Utilities/Platform.h"

#include "Modals/MaterialEditor.h"

#include "TAGE/AssetManager/AssetManager.h"
#include "TAGE/AssetManager/AssetSerializer.h"

#include <fstream>

namespace TAGE::Editor {
	ContentBrowserPanel::ContentBrowserPanel()
	{
		_DirectoryIcon = TAGE::TextureImporter::LoadTexture2D("Assets/textures/Icons/Folder.png");
		_FileIcon =      TAGE::TextureImporter::LoadTexture2D("Assets/textures/Icons/File.png");
	}

	void ContentBrowserPanel::Init()
	{
		_BaseDirectory = Project::GetAssetDirectory();
		_CurrentDirectory = _BaseDirectory;

		RefreshAssetTree();
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (_CurrentDirectory != std::filesystem::path(_BaseDirectory))
		{
			ImGui::SameLine();
			if (ImGui::Button("<-"))
			{
				_CurrentDirectory = _CurrentDirectory.parent_path();
				RefreshAssetTree();
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Import Asset")) {
			std::filesystem::path filepath = Platform::FileDialog::OpenFile("All Files (*.*)\0*.*\0");
			if (!filepath.empty()) {
				std::filesystem::path absFile = std::filesystem::absolute(filepath);
				std::filesystem::path targetPath = Project::GetAssetDirectory() / std::filesystem::relative(_CurrentDirectory, Project::GetAssetDirectory()) / filepath.filename();

				LOG_INFO("Importing from: {}", filepath.string());
				LOG_INFO("Target path: {}", targetPath.string());

				Project::GetActive()->GetEditorAssetManager()->ImportAsset(filepath, targetPath);
				RefreshAssetTree();
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("New Folder")) {
			ImGui::OpenPopup("Create New Folder");
		}

		float cellSize = _ThumbnailSize + _Padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& entry : _DirectoryEntries) {
			MEM::Ref<TARE::Texture2D> icon = entry.IsDirectory ? _DirectoryIcon : _FileIcon;
			std::string item = entry.Path.filename().string();

			ImGui::PushID(item.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			if (ImGui::ImageButton("##ITEM", (ImTextureID)(void*)icon->GetID(), { _ThumbnailSize, _ThumbnailSize }, { 1, 0 }, { 0, 1 })) {
				if (entry.Path.extension() == ".tmat") {
					MaterialEditor::OpenModal(entry.Handle);
				}
			}

			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Rename")) {
					static char renameBuffer[128];
					strcpy(renameBuffer, entry.Path.filename().string().c_str());
					ImGui::OpenPopup("Rename Item");
				}

				if (ImGui::MenuItem("Delete")) {
					std::filesystem::path target = _CurrentDirectory / entry.Path.filename();
					if (std::filesystem::exists(target)) {
						std::filesystem::remove_all(target);
						RefreshAssetTree();
					}
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("Rename Item")) {
				static char renameBuffer[128];
				ImGui::InputText("New Name", renameBuffer, sizeof(renameBuffer));
				if (ImGui::Button("Rename")) {
					std::filesystem::path oldPath = _CurrentDirectory / entry.Path.filename();
					std::filesystem::path newPath = _CurrentDirectory / renameBuffer;
					if (std::filesystem::exists(oldPath) && !std::filesystem::exists(newPath)) {
						std::filesystem::rename(oldPath, newPath);
						RefreshAssetTree();
					}
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropSource())
			{
				AssetHandle handle = entry.Handle;
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(AssetHandle));
				ImGui::Text("%s", entry.Path.stem().string().c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.IsDirectory)
				{
					_CurrentDirectory = entry.Path;
					RefreshAssetTree();
				}
			}

			ImGui::TextWrapped(item.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		if (ImGui::BeginPopupContextWindow("EmptySpaceContext", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Create Script")) {
				std::filesystem::path scriptPath = _CurrentDirectory / "NewScript.cs";
				if (!std::filesystem::exists(scriptPath)) {
					std::ofstream(scriptPath.string());
					RefreshAssetTree();
				}
			}

			if (ImGui::MenuItem("Create Material")) {
				std::filesystem::path matPath = _CurrentDirectory / "NewMaterial.tmat";
				if (!std::filesystem::exists(matPath)) {
					MEM::Ref<TARE::Material> newMat = MEM::MakeRef<TARE::Material>();
					MaterialSerializer serializer;
					serializer.Serialize(newMat, matPath);
					std::filesystem::path relativePath = std::filesystem::relative(matPath, Project::GetAssetDirectory());
					Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);
					RefreshAssetTree();
				}
			}

			if (ImGui::MenuItem("Create Folder")) {
				ImGui::OpenPopup("Create New Folder");
				RefreshAssetTree();
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("Create New Folder")) {
			static char folderName[128] = "NewFolder";
			ImGui::InputText("Folder Name", folderName, sizeof(folderName));
			if (ImGui::Button("Create")) {
				std::filesystem::path newFolder = _CurrentDirectory / folderName;
				if (!std::filesystem::exists(newFolder)) {
					std::filesystem::create_directory(newFolder);
					RefreshAssetTree();
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		ImGui::End();

		MaterialEditor::Render();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		_DirectoryEntries.clear();

		for (const auto& entry : std::filesystem::directory_iterator(_CurrentDirectory))
		{
			DirectoryEntry dirEntry;
			dirEntry.Path = entry.path();
			dirEntry.IsDirectory = entry.is_directory();

			if (!dirEntry.IsDirectory)
			{
				auto handle = Project::GetActive()->GetEditorAssetManager()->TryToGetLoadedAssetFromPath(
					std::filesystem::relative(dirEntry.Path, Project::GetAssetDirectory()));
				dirEntry.Handle = handle;
			}

			_DirectoryEntries.push_back(dirEntry);
		}

		std::sort(_DirectoryEntries.begin(), _DirectoryEntries.end(),
			[](const DirectoryEntry& a, const DirectoryEntry& b) {
				if (a.IsDirectory != b.IsDirectory)
					return a.IsDirectory > b.IsDirectory;
				return a.Path.filename().string() < b.Path.filename().string();
			});
	}
}