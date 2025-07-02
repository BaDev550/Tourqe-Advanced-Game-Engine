#pragma once
#include <filesystem>
#include "TARE/Texture/Texture.h"
#include "TAGE/Utilities/Memory.h"
#include "ThumbnailRenderer.h"

namespace TAGE::Editor {
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		void DrawNavigationBar();
		void DrawDirectoryContents();
		void DrawDirectoryEntry(const std::filesystem::directory_entry& directoryEntry);

		std::filesystem::path _CurrentDirectory;
		MEM::Ref<TARE::Texture2D> _DirectoryIcon;
		MEM::Ref<TARE::Texture2D> _FileIcon;
		ThumbnailRenderer _ThumbnailRenderer;

		float _Padding = 16.0f;
		float _ThumbnailSize = 80.0f;
	};
}