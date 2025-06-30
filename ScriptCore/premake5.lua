project "ScriptCore"
   kind "SharedLib"
   language "C#"
   dotnetframework "4.7.2"

   targetdir ("../Editor/Resources/Scripts")
   objdir ("../Editor/Resources/Scripts/Intermadiates")

   files {
       "src/**.cs",
       "properties/**.cs"
   }

   filter "configurations:Debug"
       runtime "Debug"
       optimize "Off"
       symbols "Default"

   filter "configurations:Release"
       runtime "Release"
       optimize "on"
