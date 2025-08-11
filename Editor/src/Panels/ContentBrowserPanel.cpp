#include "ContentBrowserPanel.h"
#include "imgui.h"
#include "TAGE/World/Scene/SceneSerializer.h"
#include "TAGE/Project/Project.h"

namespace TAGE::Editor {
	ContentBrowserPanel::ContentBrowserPanel()
	{
		_DirectoryIcon = TARE::Texture2D::Create(); _DirectoryIcon->LoadTexture("Assets/textures/Icons/Folder.png");
		_FileIcon = TARE::Texture2D::Create(); _FileIcon->LoadTexture("Assets/textures/Icons/File.png");
	}

	void ContentBrowserPanel::Init()
	{
		_BaseDirectory = Project::GetAssetDirectory();
		_CurrentDirectory = _BaseDirectory;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (_CurrentDirectory != std::filesystem::path(_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				_CurrentDirectory = _CurrentDirectory.parent_path();
			}
		}
		float cellSize = _ThumbnailSize + _Padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());
			MEM::Ref<TARE::Texture2D> icon = directoryEntry.is_directory() ? _DirectoryIcon : _FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton("#ICON", (ImTextureID)(void*)icon->GetID(), { _ThumbnailSize, _ThumbnailSize}, {1, 0}, {0, 1});

			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				std::string pathStr = relativePath.string();
				ImGui::SetDragDropPayload(
					"CONTENT_BROWSER_ITEM",
					pathStr.c_str(),
					pathStr.size() + 1
				);
				ImGui::Text("%s", relativePath.stem().string().c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					_CurrentDirectory /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}
}