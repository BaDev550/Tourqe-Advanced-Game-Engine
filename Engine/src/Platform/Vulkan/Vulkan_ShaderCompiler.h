#pragma once
#include <shaderc/shaderc.hpp>
#include <vector>
#include <string>

namespace TARE {
	class Vulkan_ShaderCompiler {
	public:
		std::vector<uint> CompileShaderToSPRIV(const std::string& source, shaderc_shader_kind kind, const std::string& name);
	};
}