#pragma once
#include <string>
#include <unordered_set>

namespace TAGE::Renderer {
	class ShaderPreprocessor {
	public:
		static std::string Preprocess(const std::string& filepath, std::unordered_set<std::string>* includedFiles = nullptr);
	};
}
