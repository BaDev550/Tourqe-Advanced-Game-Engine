#include "tagepch.h"
#include "OpenGL_ShaderCompiler.h"

#include <GLAD/glad.h>
#include <shaderc/shaderc.hpp>

namespace TARE::OpenGL {
    shaderc_shader_kind ShaderStageToShaderC(const std::string& stage) {
        if (stage == "vert") return shaderc_vertex_shader;
        if (stage == "frag") return shaderc_fragment_shader;
        if (stage == "geom") return shaderc_geometry_shader;
        return shaderc_glsl_infer_from_source;
    }

    GLenum ShaderStageToGLenum(const std::string& stage) {
        if (stage == "vert") return GL_VERTEX_SHADER;
        if (stage == "frag") return GL_FRAGMENT_SHADER;
        if (stage == "geom") return GL_GEOMETRY_SHADER;
        return 0;
    }

    std::string OpenGL_ShaderCompiler::PreprocessIncludes(const std::string& filepath, std::unordered_set<std::string>* includedFiles)
    {
        std::unordered_set<std::string> localIncludes;
        if (!includedFiles) includedFiles = &localIncludes;

        if (includedFiles->count(filepath)) return "";
        includedFiles->insert(filepath);

        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open shader file: " << filepath << std::endl;
            return "";
        }

        std::stringstream output;
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("#include") != std::string::npos) {
                size_t firstQuote = line.find("\"");
                size_t lastQuote = line.find_last_of("\"");
                if (firstQuote != std::string::npos && lastQuote != std::string::npos) {
                    std::string includePath = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                    std::filesystem::path fullPath = std::filesystem::path(filepath).parent_path() / includePath;
                    output << PreprocessIncludes(fullPath.string(), includedFiles);
                }
            }
            else {
                output << line << '\n';
            }
        }
        return output.str();
    }

    std::string OpenGL_ShaderCompiler::PreprocessIncludesForStage(
        const std::string& stageSource,
        const std::filesystem::path& currentDir,
        std::unordered_set<std::string>* includedFiles
    ) {
        std::unordered_set<std::string> localIncludes;
        if (!includedFiles)
            includedFiles = &localIncludes;

        std::stringstream input(stageSource);
        std::stringstream output;
        std::string line;

        while (std::getline(input, line)) {
            if (line.find("#include") != std::string::npos) {
                size_t firstQuote = line.find("\"");
                size_t lastQuote = line.find_last_of("\"");
                if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote) {
                    std::string includePath = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                    std::filesystem::path fullPath = currentDir / includePath;

                    if (includedFiles->count(fullPath.string()) == 0) {
                        includedFiles->insert(fullPath.string());
                        std::ifstream includeFile(fullPath);
                        if (!includeFile.is_open()) {
                            std::cerr << "Failed to open include file: " << fullPath << std::endl;
                            continue;
                        }

                        std::stringstream includeBuffer;
                        includeBuffer << includeFile.rdbuf();
                        output << PreprocessIncludesForStage(includeBuffer.str(), fullPath.parent_path(), includedFiles);
                    }
                }
            }
            else {
                output << line << '\n';
            }
        }

        return output.str();
    }

    std::unordered_map<std::string, std::string> OpenGL_ShaderCompiler::SplitShaderStages(const std::string& filepath) {
        std::unordered_map<std::string, std::string> sources;
        std::ifstream file(filepath);
        if (!file.is_open()) return sources;

        std::string line;
        std::stringstream buffer;
        std::string currentStage;

        while (std::getline(file, line)) {
            if (line.find("#pragma stage:vert") != std::string::npos) {
                FlushStage(sources, buffer, currentStage, filepath);
                currentStage = "vert";
            }
            else if (line.find("#pragma stage:frag") != std::string::npos) {
                FlushStage(sources, buffer, currentStage, filepath);
                currentStage = "frag";
            }
            else if (line.find("#pragma stage:geom") != std::string::npos) {
                FlushStage(sources, buffer, currentStage, filepath);
                currentStage = "geom";
            }
            else {
                buffer << line << '\n';
            }
        }
        FlushStage(sources, buffer, currentStage, filepath);
        return sources;
    }

    void OpenGL_ShaderCompiler::FlushStage(std::unordered_map<std::string, std::string>& sources, std::stringstream& buffer, const std::string& currentStage, const std::string& filepath) {
        if (!currentStage.empty() && buffer.str().length() > 0) {
            std::string stageCode = buffer.str();
            std::filesystem::path baseDir = std::filesystem::path(filepath).parent_path();
            stageCode = PreprocessIncludesForStage(stageCode, baseDir);
            sources[currentStage] = stageCode;
            buffer.str("");
            buffer.clear();
        }
    }

    bool OpenGL_ShaderCompiler::Compile()
    {
        auto sources = SplitShaderStages(_ShaderPath);
        if (sources.empty()) return false;

        std::unordered_map<GLenum, GLuint> shaders;
        for (auto& [stage, source] : sources) {
            shaderc_shader_kind kind = ShaderStageToShaderC(stage);
            shaderc::Compiler compiler;
            shaderc::CompileOptions options;

            options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
            options.SetWarningsAsErrors();

            shaderc::SpvCompilationResult result =
                compiler.CompileGlslToSpv(source, kind, _ShaderPath.c_str(), options);

            if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
                std::cerr << "Shader compilation error (" << stage << "): "
                    << result.GetErrorMessage() << std::endl;
                return false;
            }

            GLuint shader = glCreateShader(ShaderStageToGLenum(stage));

            std::vector<uint32_t> spirv(result.cbegin(), result.cend());
            glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, spirv.data(), spirv.size() * sizeof(uint32_t));
            glSpecializeShader(shader, "main", 0, nullptr, nullptr);

            shaders[ShaderStageToGLenum(stage)] = shader;
        }

        _Program = glCreateProgram();
        for (auto& [type, shader] : shaders)
            glAttachShader(_Program, shader);

        glLinkProgram(_Program);

        GLint success;
        glGetProgramiv(_Program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[1024];
            glGetProgramInfoLog(_Program, 1024, nullptr, infoLog);
            std::cerr << "Program linking error: " << infoLog << std::endl;
            return false;
        }

        for (auto& [type, shader] : shaders)
            glDeleteShader(shader);

        return true;
    }
}