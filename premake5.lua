workspace "Banan2D"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Glad"] = "%{wks.location}/Banan2D/vendor/Glad/include"
IncludeDir["imgui"] = "%{wks.location}/Banan2D/vendor/imgui"
IncludeDir["stb_image"] = "%{wks.location}/Banan2D/vendor/stb_image"
IncludeDir["glm"] = "%{wks.location}/Banan2D/vendor/glm"

group "Dependencies"
	include "Banan2D/vendor/Glad"
	include "Banan2D/vendor/imgui"
group ""

include "Banan2D"
include "Sandbox"
--include "Testing"