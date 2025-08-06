#include "tagepch.h"
#include "Vulkan_ShaderCompiler.h"

namespace TARE {
	std::vector<uint> Vulkan_ShaderCompiler::CompileShaderToSPRIV(const std::string& source, shaderc_shader_kind kind, const std::string& name)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetOptimizationLevel(shaderc_optimization_level_size);
		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, name.c_str(), options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			LOG_ERROR("Shader compilation failed: {}", result.GetErrorMessage());
			return std::vector<uint>();
		}

		return { result.cbegin(), result.cend() };
	}
}