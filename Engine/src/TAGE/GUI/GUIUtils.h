#pragma once
#include <string>
#include <vector>

#include "TAGE/Utilities/Memory.h"
#include "TAGE/AssetManager/AssetManager.h"

namespace TAGE {
	class GUI
	{
	public:
		static bool DrawVector3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f);
		static bool DrawVector2Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f);
		static bool DrawColorControl(const std::string& label, glm::vec4& color);
		static bool DrawColorControl(const std::string& label, glm::vec3& color);

		static bool ComboBox(const std::string& label, int& currentItem, const std::vector<std::string>& items, const std::string& defaultSelectText = "Select...");
		static bool ComboBox(const std::string& label, AssetHandle& currentItem, AssetType type, AssetHandle currentHandle = 0, const std::string& defaultSelectText = "Select...");
	};
}
