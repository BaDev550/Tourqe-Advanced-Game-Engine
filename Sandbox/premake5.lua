project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs {
        "src/",
        "../Engine/vendor/spdlog/",
        "../%{IncludeDir.Engine}",
        "../Engine/%{IncludeDir.GLFW}",
        "../Engine/%{IncludeDir.GLAD}",
        "../Engine/%{IncludeDir.ImGui}",
        "../Engine/%{IncludeDir.Assimp}",
        "../Engine/%{IncludeDir.glm}",
    }

    links {
        "Engine"
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
        postbuildcommands {
            "{COPY} assets %{cfg.targetdir}/assets",
            "{COPY} ../Engine/shaders %{cfg.targetdir}/shaders",
            "{COPY} ../Engine/assets %{cfg.targetdir}/assets"
        }
