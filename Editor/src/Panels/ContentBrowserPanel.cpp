#include "ContentBrowserPanel.h"
#include "imgui.h"
#include "TAGE/World/Scene/SceneSerializer.h"
#include "TAGE/Project/Project.h"
#include "TAGE/AssetManager/Importers/TextureImporter.h"

#include "Modals/MaterialEditor.h"

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
		_TreeNodes.push_back(TreeNode("/", 0));

		RefreshAssetTree();
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		const char* label = _Mode == Mode::Asset ? "Asset" : "File";
		if (ImGui::Button(label)) {
			_Mode = _Mode == Mode::Asset ? Mode::Filesystem : Mode::Asset;
		}

		if (_CurrentDirectory != std::filesystem::path(_BaseDirectory))
		{
			ImGui::SameLine();
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

		if (_Mode == Mode::Asset){
			TreeNode* node = &_TreeNodes[0];

			auto currentDir = std::filesystem::relative(_CurrentDirectory, Project::GetAssetDirectory());
			for (const auto& p : currentDir) {
				if (node->Path == currentDir)
					break;

				if (node->Children.find(p) != node->Children.end()) {
					node = &_TreeNodes[node->Children[p]];
					continue;
				}
				else {
					break;
				}
			}

			for (const auto& [item, index] : node->Children) {
				bool isDirectory = std::filesystem::is_directory(Project::GetAssetDirectory() / item);
				MEM::Ref<TARE::Texture2D> icon = isDirectory ? _DirectoryIcon : _FileIcon;
				std::string itemStr = item.generic_string();

				ImGui::PushID(item.c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

				if (ImGui::ImageButton("##ITEM", (ImTextureID)(void*)icon->GetID(), { _ThumbnailSize, _ThumbnailSize }, { 1, 0 }, { 0, 1 })) {
					if (isDirectory) {
						_CurrentDirectory /= item.filename();
					}
					else if (item.extension() == ".tmat") {
						auto material = AssetManager::GetAsset<TARE::Material>(_TreeNodes[index].Handle);
						MaterialEditor::OpenModal(_TreeNodes[index].Handle);
					}
				}

				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Delete")) {

					}
					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource())
				{
					AssetHandle handle = _TreeNodes[index].Handle;
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(AssetHandle));
					ImGui::Text("%s", item.stem().string().c_str());
					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (isDirectory)
						_CurrentDirectory /= item.filename();
				}

				ImGui::TextWrapped(itemStr.c_str());

				ImGui::NextColumn();

				ImGui::PopID();
			}
		}
		else {
			for (auto& directoryEntry : std::filesystem::directory_iterator(_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				std::string filenameString = path.filename().string();

				ImGui::PushID(filenameString.c_str());
				MEM::Ref<TARE::Texture2D> icon = directoryEntry.is_directory() ? _DirectoryIcon : _FileIcon;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton("#ICON", (ImTextureID)(void*)icon->GetID(), { _ThumbnailSize, _ThumbnailSize }, { 1, 0 }, { 0, 1 });

				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Import")) {
						auto relativePath = std::filesystem::relative(path, Project::GetActive()->GetAssetDirectory());
						Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);
						RefreshAssetTree();
					}
					ImGui::EndPopup();
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
		}

		ImGui::Columns(1);

		ImGui::End();

		MaterialEditor::Render();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		const auto& assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
		for (const auto& [handle, metadata] : assetRegistry) {
			uint currentNodeIndex = 0;

			for (const auto& p : metadata.FilePath) {
				auto it = _TreeNodes[currentNodeIndex].Children.find(p.generic_string());
				if (it != _TreeNodes[currentNodeIndex].Children.end()) {
					currentNodeIndex = it->second;
				}
				else {
					TreeNode newNode(p, handle);
					newNode.Parent = currentNodeIndex;
					_TreeNodes.push_back(newNode);

					_TreeNodes[currentNodeIndex].Children[p] = _TreeNodes.size() - 1;
					currentNodeIndex = _TreeNodes.size() - 1;

				}
			}
		}
	}
}