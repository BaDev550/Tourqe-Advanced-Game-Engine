workspace "TAGE"
   architecture "x64"
   startproject "Editor"

   configurations
   {
      "Debug",
      "Release"
   }

   outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
   VULKAN_SDK = os.getenv("VULKAN_SDK")

    IncludeDir = {}
    IncludeDir["Engine"] =        "%{wks.location}/Engine/src/"
    IncludeDir["GLFW"] =          "%{wks.location}/Engine/vendor/GLFW/include"
    IncludeDir["GLAD"] =          "%{wks.location}/Engine/vendor/GLAD/include"
    IncludeDir["Assimp"] =        "%{wks.location}/Engine/vendor/assimp/include"
    IncludeDir["glm"] =           "%{wks.location}/Engine/vendor/glm/"
    IncludeDir["ImGui"] =         "%{wks.location}/Engine/vendor/imgui"
    IncludeDir["ImGizmo"] =       "%{wks.location}/Engine/vendor/ImGuizmo"
    IncludeDir["entt"] =          "%{wks.location}/Engine/vendor/entt/include"
    IncludeDir["BulletPhysics"] = "%{wks.location}/Engine/vendor/bullet-physics/src"
    IncludeDir["yaml"] =          "%{wks.location}/Engine/vendor/yaml-cpp/include"
    IncludeDir["mono"] =          "%{wks.location}/Engine/vendor/mono/include"
    IncludeDir["meshoptimizer"] = "%{wks.location}/Engine/vendor/meshoptimizer/"
    IncludeDir["Vulkan"] = "%{VULKAN_SDK}/Include"
    IncludeDir["shaderc"] = "%{wks.location}/Engine/vendor/shaderc/include"
    IncludeDir["SPIRV_Cross"] = "%{wks.location}/Engine/vendor/SPIRV-Cross"

    LibaryDir = {}
    LibaryDir["mono"] = "%{wks.location}/Engine/vendor/mono/lib/%{cfg.buildcfg}"

    Library = {}
    Library["mono"] = "%{LibaryDir.mono}/libmono-static-sgen.lib";
    Library["WinSock"] = "Ws2_32.lib";
    Library["Winmm"] = "winmm.lib";
    Library["WinVersion"] = "version.lib";
    Library["WinBcrypt"] = "Bcrypt.lib";
    Library["Vulkan"] = "%{VULKAN_SDK}/Lib"
    Library["VulkanSDK"] = "%{VULKAN_SDK}/Lib/vulkan-1.lib"
    Library["ShaderC_Debug"] = "%{Library.Vulkan}/shaderc_shared.lib"
    Library["SPIRV_Cross_Debug"] = "%{Library.Vulkan}/spirv-cross-core.lib"
    Library["SPIRV_Cross_GLSL_Debug"] = "%{Library.Vulkan}/spirv-cross-glsl.lib"
    Library["SPIRV_Tools_Debug"] = "%{Library.Vulkan}/SPIRV-Tools.lib"
    Library["ShaderC_Release"] = "%{Library.Vulkan}/shaderc_shared.lib"
    Library["SPIRV_Cross_Release"] = "%{Library.Vulkan}/spirv-cross-core.lib"
    Library["SPIRV_Cross_GLSL_Release"] = "%{Library.Vulkan}/spirv-cross-glsl.lib"

    group "Dependencies"
        include "Engine/vendor/imgui"
        include "Engine/vendor/ImGuizmo"
        include "Engine/vendor/GLAD"
        include "Engine/vendor/GLFW"
        include "Engine/vendor/assimp"
        include "Engine/vendor/yaml-cpp"
        include "Engine/vendor/bullet-physics/"
        include "Engine/vendor/meshoptimizer/"
    group ""

    group "Engine"
        include "Engine/"
        include "ScriptCore/"
    group ""

    group "Editor"
        include "Editor/"
    group ""
