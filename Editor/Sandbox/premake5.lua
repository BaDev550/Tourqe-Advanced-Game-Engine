local TAGERootDir = "E:/TAGE/"
workspace "Sandbox"
    architecture "x86_64"
    startproject "Sandbox"

    configurations { "Debug", "Release" }
    flags { "MultiProcessorCompile" }
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Sandbox"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.7.2"
    targetdir ("Binaries")
    objdir ("Intermediates")
    links { "ScriptCore" }
    files { "Assets/**.cs" }

    filter "configurations:Debug"
        runtime "Debug"
        optimize "Off"
        symbols "Default"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

group "TAGE"
    include (TAGERootDir .. "ScriptCore/")
group ""
