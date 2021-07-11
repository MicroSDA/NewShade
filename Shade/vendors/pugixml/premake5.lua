group "Engine/Vendors"
project "pugixml"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

	files
	{
		"pugixml.hpp",
		"pugiconfig.hpp",
		"pugixml.cpp"
	}
	
	includedirs {
		"%{prj.name}/"
	}
	
	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"