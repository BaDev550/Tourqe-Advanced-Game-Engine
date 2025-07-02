workspace "TAGE"
   architecture "x64"
   startproject "Editor"

   configurations
   {
      "Debug",
      "Release"
   }

   outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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

    LibaryDir = {}
    LibaryDir["mono"] = "%{wks.location}/Engine/vendor/mono/lib/%{cfg.buildcfg}"

    Library = {}
    Library["mono"] = "%{LibaryDir.mono}/libmono-static-sgen.lib";
    Library["WinSock"] = "Ws2_32.lib";
    Library["Winmm"] = "winmm.lib";
    Library["WinVersion"] = "version.lib";
    Library["WinBcrypt"] = "Bcrypt.lib";
    
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
