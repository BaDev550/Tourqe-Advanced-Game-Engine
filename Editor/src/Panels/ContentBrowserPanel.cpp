#include "ContentBrowserPanel.h"
#include "imgui.h"
#include "TAGE/World/Scene/SceneSerializer.h"

namespace TAGE::Editor {
	static const std::filesystem::path s_AssetsPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel() : _CurrentDirectory(s_AssetsPath) {
		_DirectoryIcon = TARE::Texture2D::Create();
		_DirectoryIcon->LoadTexture("assets/textures/Icons/Folder.png");
		_FileIcon = TARE::Texture2D::Create();
		_FileIcon->LoadTexture("assets/textures/Icons/File.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		_ThumbnailRenderer.OnUpdate();

		ImGui::Begin("Content Browser");

		float padding = 16.0f;
		float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		if (_CurrentDirectory != std::filesystem::path(s_AssetsPath)) {
			if (ImGui::Button("<-")) {
				_CurrentDirectory = _CurrentDirectory.parent_path();
			}
		}

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(_CurrentDirectory)) {
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetsPath);
			std::string filenameStr = relativePath.filename().string();

			ImGui::PushID(filenameStr.c_str());

			uint iconID = 0;
			bool flipUV = false;

			if (directoryEntry.is_directory()) {
				iconID = _DirectoryIcon->GetID();
			}
			else {
				std::string ext = path.extension().string();
				if (ext == ".fbx" || ext == ".obj" || ext == ".gltf") {
					_ThumbnailRenderer.SubmitModel(path);
					iconID = _ThumbnailRenderer.GetThumbnail(path);
					flipUV = true;
				}
				else {
					iconID = _FileIcon->GetID();
				}
			}

			if (flipUV) {
				ImGui::ImageButton("Asset", (ImTextureID)iconID, ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0));
			}
			else {
				ImGui::ImageButton("Asset", (ImTextureID)iconID, ImVec2(thumbnailSize, thumbnailSize));
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (directoryEntry.is_directory())
					_CurrentDirectory /= path.filename();
				else {

					// file
				}
			}

			ImGui::TextWrapped(filenameStr.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);
		ImGui::End();
	}
}