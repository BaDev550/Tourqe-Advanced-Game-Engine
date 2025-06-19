#include "tagepch.h"
#include "ShaderPreprocessor.h"

namespace fs = std::filesystem;

namespace TARE {
	std::string ShaderPreprocessor::Preprocess(const std::string& filepath, std::unordered_set<std::string>* includedFiles) {
		if (!includedFiles)
			includedFiles = new std::unordered_set<std::string>();

		if (includedFiles->count(filepath))
			return "";

		includedFiles->insert(filepath);

		std::ifstream file(filepath);
		if (!file.is_open()) {
			LOG_ERROR("Failed to open shader file: {0}", filepath);
			return "";
		}

		std::stringstream processed;
		std::string line;
		while (std::getline(file, line)) {
			if (line.find("#include") != std::string::npos) {
				size_t firstQuote = line.find("\"");
				size_t lastQuote = line.find_last_of("\"");

				if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote) {
					std::string includePath = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
					fs::path base = fs::path(filepath).parent_path();
					fs::path fullPath = base / includePath;
					processed << Preprocess(fullPath.string(), includedFiles);
				}
			}
			else {
				processed << line << '\n';
			}
		}
		return processed.str();
	}
}
