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
    IncludeDir["Engine"] = "Engine/src/"
    IncludeDir["GLFW"] =   "vendor/GLFW/include"
    IncludeDir["GLAD"] =   "vendor/GLAD/include"
    IncludeDir["Assimp"] = "vendor/assimp/include"
    IncludeDir["glm"] =    "vendor/glm/"
    IncludeDir["ImGui"] =  "vendor/imgui"
    IncludeDir["ImGizmo"] =  "vendor/ImGuizmo"
    IncludeDir["entt"] =  "vendor/entt/include"
    IncludeDir["yaml"] =  "vendor/yaml-cpp/include"

    group "Dependencies"
        include "Engine/vendor/imgui"
        include "Engine/vendor/ImGuizmo"
        include "Engine/vendor/GLAD"
        include "Engine/vendor/GLFW"
        include "Engine/vendor/assimp"
        include "Engine/vendor/yaml-cpp"
    group ""

    group "Engine"
        include "Engine/"
    group ""

    group "Editor"
        include "Editor/"
    group ""
