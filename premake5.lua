workspace "FallenAngel"
	configurations
	{
		"Debug",
		"Release",
		"Deploy" }

	project "FallenAngel"
		kind "ConsoleApp"
		language "C++"
		targetdir "bin/lib/%{cfg.buildcfg}"

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

		pchheader "./Lib/pch.hpp"
		pchsource "./Lib/pch.cpp"

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
