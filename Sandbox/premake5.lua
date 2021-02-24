project "Sandbox"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Banan2D/src",
		"%{wks.location}/Banan2D/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Banan2D"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "BGE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "BGE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "BGE_DISTRIBUTION"
		runtime "Release"
		optimize "on"