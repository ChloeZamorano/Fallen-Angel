workspace "FallenAngel"
	configurations
	{
		"Debug",
		"Release",
		"Deploy"
	}
	platforms
	{
		"Linux",
		"Windows"
	}

	project "FallenAngel"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		cdialect "C17"
		targetdir "./bin"

		files
		{
			"./Lib/main.cpp",

			"./Lib/inc/**.hpp",
			"./Lib/inc/**.inl",
			"./Lib/src/**.cpp",

			"./Lib/pch.cpp",
			"./Lib/pch.hpp"
		}

		includedirs
		{
			"./Lib/inc/",
			"./Lib/",
		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
		}
		
		pchsource "./Lib/pch.cpp"
		pchheader "pch.hpp"

		filter "platforms:Windows"
			defines { "WINDOWS" }

		filter "platforms:Linux"
			defines { "LINUX" }

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
