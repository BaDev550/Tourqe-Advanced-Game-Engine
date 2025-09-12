#include "tagepch.h"
#include "GUIUtils.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "TAGE/Project/Project.h"

#include <glm/gtc/type_ptr.hpp>

namespace TAGE {
	bool GUI::ComboBox(const std::string& label, int& currentItem, const std::vector<std::string>& items, const std::string& defaultSelectText)
	{
		bool valueChanged = false;
		if (ImGui::BeginCombo(label.c_str(), currentItem >= 0 ? items[currentItem].c_str() : defaultSelectText.c_str())) {
			for (int i = 0; i < static_cast<int>(items.size()); ++i) {
				bool selected = (i == currentItem);
				if (ImGui::Selectable(items[i].c_str(), selected)) {
					currentItem = i;
					valueChanged = true;
				}
				if (selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		return valueChanged;
	}

	bool GUI::ComboBox(const std::string& label, AssetHandle& currentItem, AssetType type, AssetHandle currentHandle, const std::string& defaultSelectText)
	{
		auto& assetManager = *Project::GetActive()->GetEditorAssetManager();
		auto Handles = assetManager.GetHandlesWithType(type);
		std::vector<AssetHandle> handles;
		std::vector<std::string> names;
		std::string currentName;

		for (const auto& h : Handles) {
			if (AssetManager::IsAssetHandleValid(h)) {
				handles.push_back(h);
				names.push_back(assetManager.GetMetadata(h).FilePath.stem().string());
			}
		}

		int selectedIndex = -1;
		if (currentHandle) {
			for (size_t i = 0; i < handles.size(); ++i) {
				if (handles[i] == currentHandle) {
					currentItem = static_cast<int>(i);
					currentName = names[i];
					break;
				}
			}
		}

		if (GUI::ComboBox(label.c_str(), selectedIndex, names, currentName.empty() ? "Select..." : currentName)) {
			if (selectedIndex >= 0 && selectedIndex < static_cast<int>(handles.size())) {
				currentItem = handles[selectedIndex];
				return true;
			}
		}
		return false;
	}
}