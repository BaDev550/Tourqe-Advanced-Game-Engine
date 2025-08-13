#pragma once
#include <filesystem>

namespace TAGE::Editor {
	class AssetManagerPanel
	{
	public:
		AssetManagerPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path _CurrentDirectory;
		std::filesystem::path _BaseDirectory;
	};
}