workspace "Shade"
	architecture "x64"

	configurations {
		"Debug",
		"Release"
	}

output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
includeDir = {}
includeDir["glfw"] = "Shade/vendors/glfw/include"


include "Shade/vendors/glfw/"

project "Shade"
	location	"Shade"
	kind		"SharedLib"
	language	"C++"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir    ("bin-int/" .. output_dir .. "/%{prj.name}")

	pchheader "shade_pch.h"
	pchsource "Shade/include/shade_pch.cpp"

	files {
		"%{prj.name}/include/**.h",
		"%{prj.name}/include/**.cpp"
	}

	includedirs {
		"%{prj.name}/include",
		"%{prj.name}/vendors/spdlog/include",
		"%{includeDir.glfw}"
	}
	
	links {
		"glfw",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines {
			"SHADE_BUILD_DLL"
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. output_dir .. "/Editor")
		}

	filter "configurations:Debug"
		defines "SHADE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "SHADE_RELEASE"
		optimize "On"

project "Editor"
	location	"Editor"
	kind		"ConsoleApp"
	language	"C++"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir    ("bin-int/" .. output_dir .. "/%{prj.name}")

	files {
		"%{prj.name}/include/**.h",
		"%{prj.name}/include/**.cpp"
	}

	includedirs {
		"%{prj.name}/include",
		"Shade/vendors/spdlog/include",
		"Shade/include",
	}

	links {
		"Shade"
	}
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines {
			"SHADE_WINDOWS_PLATFORM"
		}

	filter "configurations:Debug"
		defines "SHADE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "SHADE_RELAESE"
		optimize "On"
