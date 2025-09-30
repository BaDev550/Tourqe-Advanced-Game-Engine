#pragma once

#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace TARE::OpenGL {
    class OpenGL_ShaderCompiler
    {
    public:
        OpenGL_ShaderCompiler(const std::string& shaderPath)
            : _ShaderPath(shaderPath) {
        }

        bool Compile();

        uint GetProgram() const { return _Program; }
    private:
        std::string _ShaderPath;
        std::string _ShaderSource;
        uint _Program = 0;

        std::string PreprocessIncludes(const std::string& filepath, std::unordered_set<std::string>* includedFiles = nullptr);
        std::string PreprocessIncludesForStage(
            const std::string& stageSource,
            const std::filesystem::path& currentDir,
            std::unordered_set<std::string>* includedFiles = nullptr
        );

        std::unordered_map<std::string, std::string> SplitShaderStages(const std::string& filepath);
        void FlushStage(std::unordered_map<std::string, std::string>& sources,
            std::stringstream& buffer,
            const std::string& currentStage,
            const std::string& filepath);
    };
}

