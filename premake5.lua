workspace "FallenAngel"
	configurations
	{
		"Debug",
		"Release",
		"Deploy"
	}

	project "FallenAngel"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		cdialect "C17"
		toolset "clang"
		targetdir "bin"

		files
		{
			"./Lib/main.cpp",

			"./Lib/inc/**.hpp",
			"./Lib/inc/**.tpp",
			"./Lib/src/**.cpp"
		}

		includedirs
		{
			"./Lib/inc/",
			"./Lib/"
		}
		
		pchheader "pch.hpp"

		filter "configurations:Debug"
			defines { "DEBUG" }
			symbols "On"
			optimize "Off"

		filter "configurations:Release"
			defines { "RELEASE" }
			symbols "On"
			optimize "On"

		filter "configurations:Deploy"
			defines { "DEPLOY" }
			symbols "On"
			optimize "On"
