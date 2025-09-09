local TAGERootDir = "E:/TAGE/"
workspace "Hellstorm"
    architecture "x86_64"
    startproject "Hellstorm"

    configurations { "Debug", "Release" }
    flags { "MultiProcessorCompile" }
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Hellstorm"
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
