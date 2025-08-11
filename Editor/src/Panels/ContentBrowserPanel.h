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
		void Init();

		void OnImGuiRender();
	private:
		std::filesystem::path _CurrentDirectory;
		std::filesystem::path _BaseDirectory;

		MEM::Ref<TARE::Texture2D> _DirectoryIcon;
		MEM::Ref<TARE::Texture2D> _FileIcon;
		
		float _Padding = 19.0f;
		float _ThumbnailSize = 66.0f;
	};
}