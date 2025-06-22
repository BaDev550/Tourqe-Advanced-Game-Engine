project "Engine"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"

   targetdir ("bin/" .. outputdir .. "/%{prj.name}")
   objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "tagepch.h"
    pchsource "src/tagepch.cpp"

   files {
       "src/**.h",
       "src/**.cpp",
       "shaders/**.glsl",
       "vendor/stb/**.h",
       "vendor/stb/**.cpp",
       "vendor/glm/glm/**.hpp",
       "vendor/glm/glm/**.inl",
   }

   includedirs {
        "src",
        "vendor",
        "vendor/spdlog/",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGizmo}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml}"
   }

   defines 
   {
       "_CRT_SECURE_NO_WARNINGS"
   }

   links {
        "opengl32.lib",
        "GLAD",
        "GLFW",
        "assimp",
        "ImGui",
        "ImGuiGizmo",
        "yaml-cpp",
        "d3d11.lib",
        "dxgi.lib",
        "d3dcompiler.lib"
       }

   buildoptions "/utf-8"
   filter "system:windows"
       systemversion "latest"

   filter "configurations:Debug"
       runtime "Debug"
       symbols "on"

   filter "configurations:Release"
       runtime "Release"
       optimize "on"
