workspace "Shade"
	architecture "x64"

	configurations {
		"Debug",
		"Release"
	}

output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
includeDir = {}
includeDir["glfw"]   	= "Shade/vendors/glfw/include"
includeDir["glad"]   	= "Shade/vendors/glad/include"
includeDir["glm"]  	 	= "Shade/vendors/glm"
includeDir["ImGui"]  	= "Shade/vendors/ImGui"
includeDir["pugixml"]   = "Shade/vendors/pugixml"
includeDir["ImGuizmo"]  = "Shade/vendors/ImGuizmo"
includeDir["Yaml"]  	= "Shade/vendors/Yaml/include"


include "Shade/vendors/glfw"
include "Shade/vendors/glad"
include "Shade/vendors/ImGui"
include "Shade/vendors/pugixml"
include "Shade/vendors/Yaml"

group "Engine"
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
		"%{prj.name}/include/**.cpp",
		"%{prj.name}/vendors/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendors/ImGuizmo/ImGuizmo.cpp"
	}
	
	includedirs {
		"%{prj.name}/include",
		"%{prj.name}/vendors",
		"%{prj.name}/vendors/spdlog/include",
		"%{prj.name}/vendors/entt",
		"%{includeDir.glfw}",
		"%{includeDir.glad}",
		"%{includeDir.glm}",
		"%{includeDir.ImGui}",
		"%{includeDir.pugixml}",
		"%{includeDir.ImGuizmo}",
		"%{includeDir.Yaml}"
	}
	
	links {
		"glfw",
		"glad",
		"opengl32.lib",
		"ImGui",
		"pugixml",
		"Yaml"
	}

	filter "files:%{includeDir.ImGuizmo}/**.cpp"
		flags { "NoPCH" }
		
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines {
			"SHADE_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"SHADE_WINDOWS_PLATFORM",
			"ENTT_API_EXPORT"
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
		
group "Clients"	
project "Sandbox"
	location	"Sandbox"
	language	"C++"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir    ("bin-int/" .. output_dir .. "/%{prj.name}")

	files {
		"%{prj.name}/include/**.h",
		"%{prj.name}/include/**.cpp"
	}

	includedirs {
		"%{prj.name}/include",
		"%{prj.name}/include/vendors",
		"Shade/vendors/spdlog/include",
		"Shade/vendors/glm/",
		"Shade/include",
		"Shade/vendors"
	}
	
	links {
		"Shade",
		"pugixml"
	}
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines {
			"SHADE_WINDOWS_PLATFORM",
			"ENTT_API_IMPORT"
		}

	filter "configurations:Debug"
		defines "SHADE_DEBUG"
		symbols "On"
		kind	"ConsoleApp"
		linkoptions '/ENTRY:"mainCRTStartup"'

	filter "configurations:Release"
		defines "SHADE_RELAESE"
		optimize "On"
		kind	 "WindowedApp"
		linkoptions '/ENTRY:"mainCRTStartup"'
		
project "Editor"
	location	"Editor"
	language	"C++"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir    ("bin-int/" .. output_dir .. "/%{prj.name}")

	files {
		"%{prj.name}/include/**.h",
		"%{prj.name}/include/**.cpp"
	}

	includedirs {
		"%{prj.name}/include",
		"%{prj.name}/include/vendors",
		"Shade/vendors/spdlog/include",
		"Shade/vendors/glm/",
		"Shade/include",
		"Shade/vendors"
	}
	
	libdirs {"%{prj.name}/libs/assimp" }

	links {
		"Shade",
		"pugixml",
		"assimp-vc142-mt"
	}
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines {
			"SHADE_WINDOWS_PLATFORM",
			"ENTT_API_IMPORT"
		}

	filter "configurations:Debug"
		defines "SHADE_DEBUG"
		symbols "On"
		kind	"ConsoleApp"
		linkoptions '/ENTRY:"mainCRTStartup"'

	filter "configurations:Release"
		defines "SHADE_RELAESE"
		optimize "On"
		kind	 "WindowedApp"
		linkoptions '/ENTRY:"mainCRTStartup"'
		
group "Clients/Scripts"			
project "Scripts"
	location	"Editor/Scripts"
	kind		"SharedLib"
	language	"C++"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir    ("bin-int/" .. output_dir .. "/%{prj.name}")

	files {
		"Editor/%{prj.name}/include/**.h",
		"Editor/%{prj.name}/include/**.cpp"
	}

	includedirs {
		"%{prj.name}/include",
		"Shade/vendors/spdlog/include",
		"Shade/vendors/glm/",
		"Shade/include",
		"Shade/vendors"
	}

	links {
		"Shade"
	}
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines {
			"ENTT_API_IMPORT"
		}

	filter "configurations:Debug"
		defines "SHADE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "SHADE_RELAESE"
		optimize "On"