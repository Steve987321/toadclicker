workspace "autoclicker"
	configurations
	{
		"Debug",
		"Release",
	}

	architecture "x64"
	startproject "Toad"

project "Toad"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin-Inter/%{cfg.buildcfg}"

	pchheader "pch.h"
	pchsource "src/pch.cpp" 

	flags 
	{
		"MultiProcessorCompile",
	}

	files 
	{
		"src/**.cpp",
		"src/**.h",
		"vendor/**.cpp", 
		"vendor/**.h",
	}

	includedirs
	{
		"src/",
		"vendor/nlohmann",
		"vendor/imgui",
	}

	links
	{
		"d3d9",
		"winmm"
	}

	filter "files:vendor/**.cpp"
		flags {"NoPCH"}
	filter {}

    filter "configurations:Debug" 
		defines 
		{
			"_DEBUG"
		}

		runtime "Debug"
		optimize "Off"
		symbols "On"
	filter {}

    filter "configurations:Release"
		defines 
		{
			"NDEBUG"
		}

		kind "WindowedApp"

		runtime "Release"
        optimize "On"
		symbols "Off"
	filter {}
