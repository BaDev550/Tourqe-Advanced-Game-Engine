#include "ContentBrowserPanel.h"
#include "imgui.h"
#include "TAGE/World/Scene/SceneSerializer.h"

namespace TAGE::Editor {
	static const std::filesystem::path s_AssetsPath = "assets";
	static const char* CONTENT_BROWSER_PAYLOAD = "CONTENT_BROWSER_ITEM";

	ContentBrowserPanel::ContentBrowserPanel() : _CurrentDirectory(s_AssetsPath) {
		_DirectoryIcon = TARE::Texture2D::Create();
		_DirectoryIcon->LoadTexture("Assets/textures/Icons/Folder.png");
		_FileIcon = TARE::Texture2D::Create();
		_FileIcon->LoadTexture("Assets/textures/Icons/File.png");
		_CSharpFileIcon = TARE::Texture2D::Create();
		_CSharpFileIcon->LoadTexture("Assets/textures/Icons/FileCsharp.png");
		_TAGESceneFileIcon = TARE::Texture2D::Create();
		_TAGESceneFileIcon->LoadTexture("Assets/textures/Icons/FileTageScene.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		_ThumbnailRenderer.OnUpdate();

		ImGui::Begin("Content Browser");

		DrawNavigationBar();
		ImGui::Separator();
		DrawDirectoryContents();

		ImGui::End();
	}

	void ContentBrowserPanel::DrawNavigationBar()
	{
		if (_CurrentDirectory != s_AssetsPath)
		{
			if (ImGui::Button("<-"))
			{
				_CurrentDirectory = _CurrentDirectory.parent_path();
			}
			ImGui::SameLine();
		}

		std::filesystem::path relativePath = std::filesystem::relative(_CurrentDirectory, s_AssetsPath);
		ImGui::Text("%s", s_AssetsPath.string().c_str());

		for (const auto& part : relativePath)
		{
			if (part.string() == ".") continue;

			ImGui::SameLine();
			ImGui::Text(">");
			ImGui::SameLine();
			std::filesystem::path partPath = s_AssetsPath / std::filesystem::path(relativePath).remove_filename() / part;

			if (ImGui::Button(part.string().c_str()))
			{
				_CurrentDirectory = partPath;
				break;
			}
		}
	}

	void ContentBrowserPanel::DrawDirectoryContents()
	{
		float cellSize = _ThumbnailSize + _Padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		if (ImGui::BeginTable("DirectoryGrid", columnCount))
		{
			for (auto& directoryEntry : std::filesystem::directory_iterator(_CurrentDirectory))
			{
				ImGui::TableNextColumn();
				DrawDirectoryEntry(directoryEntry);
			}
			ImGui::EndTable();
		}
	}

	void ContentBrowserPanel::DrawDirectoryEntry(const std::filesystem::directory_entry& directoryEntry)
	{
		const auto& path = directoryEntry.path();
		std::string filenameStr = path.filename().string();

		ImGui::PushID(filenameStr.c_str());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		uint32_t iconID = 0;
		bool flipUV = false;

		if (directoryEntry.is_directory())
		{
			iconID = _DirectoryIcon->GetID();
		}
		else
		{
			std::string ext = path.extension().string();
			//if (ext == ".fbx" || ext == ".obj" || ext == ".gltf")
			//{
			//	_ThumbnailRenderer.SubmitModel(path);
			//	iconID = _ThumbnailRenderer.GetThumbnail(path);
			//	flipUV = true;
			//}
			if (ext == ".tage")
				iconID = _TAGESceneFileIcon->GetID();
			else if (ext == ".cs")
				iconID = _CSharpFileIcon->GetID();
			else
				iconID = _FileIcon->GetID();
		}

		ImVec2 uv0 = flipUV ? ImVec2(0, 1) : ImVec2(0, 0);
		ImVec2 uv1 = flipUV ? ImVec2(1, 0) : ImVec2(1, 1);
		ImGui::ImageButton("##Icon", (ImTextureID)(uint64_t)iconID, ImVec2(_ThumbnailSize, _ThumbnailSize), uv0, uv1);

		ImGui::PopStyleColor();

		if (ImGui::BeginDragDropSource())
		{
			std::string itemPathStr = path.string();
			ImGui::SetDragDropPayload(CONTENT_BROWSER_PAYLOAD, itemPathStr.c_str(), itemPathStr.size() + 1);
			ImGui::Text("%s", filenameStr.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directoryEntry.is_directory())
			{
				_CurrentDirectory /= path.filename();
			}
			else if (path.extension() == ".tage")
			{
				LOG_INFO("Request to open scene: {0}", path.string());
			}
		}
		float textWidth = ImGui::CalcTextSize(filenameStr.c_str()).x;
		float textIndent = (_ThumbnailSize - textWidth) * 0.5f;
		if (textIndent > 0.0f)
			ImGui::Indent(textIndent);

		ImGui::TextWrapped("%s", filenameStr.c_str());

		if (textIndent > 0.0f)
			ImGui::Unindent(textIndent);

		ImGui::PopID();
	}
}