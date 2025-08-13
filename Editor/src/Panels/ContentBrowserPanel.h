#pragma once
#include <filesystem>
#include <map>
#include <set>

#include "TARE/Texture/Texture.h"
#include "TAGE/Utilities/Memory.h"
#include "ThumbnailRenderer.h"

namespace TAGE::Editor {
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void Init();

		void OnImGuiRender();
	private:
		void RefreshAssetTree();
	private:
		std::filesystem::path _CurrentDirectory;
		std::filesystem::path _BaseDirectory;

		struct TreeNode {
			std::filesystem::path Path;
			AssetHandle Handle = 0;

			uint Parent = (uint)-1;
			std::map<std::filesystem::path, uint> Children;

			TreeNode() = default;
			TreeNode(const std::filesystem::path& path, AssetHandle handle) : Path(path), Handle(handle) {}
		};
		std::vector<TreeNode> _TreeNodes;

		MEM::Ref<TARE::Texture2D> _DirectoryIcon;
		MEM::Ref<TARE::Texture2D> _FileIcon;
		
		float _Padding = 19.0f;
		float _ThumbnailSize = 66.0f;

		enum class Mode {
			Asset = 0, Filesystem = 1
		};
		Mode _Mode = Mode::Filesystem;
	};
}