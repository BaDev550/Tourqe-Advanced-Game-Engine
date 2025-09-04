project "ImGui-Node-Editor"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"builders.h",
		"crude_json.h",
		"crude_json.cpp",
		"imgui_bezier_math.h",
		"imgui_bezier_math.inl",
		"imgui_internal.h",
		"imgui_canvas.h",
		"imgui_canvas.cpp",
		"imgui_extra_math.h",
		"imgui_extra_math.inl",
		"imgui_node_editor.h",
		"imgui_node_editor.cpp",
		"imgui_node_editor_internal.h",
		"imgui_node_editor_internal.inl",
		"imgui_node_editor_api.cpp"
	}
 
	includedirs {
		".",
		"../imgui"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
