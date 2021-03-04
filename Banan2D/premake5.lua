project "Banan2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "bgepch.h"
	pchsource "src/bgepch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.glm}"
	}
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	links
	{
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "BANAN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "BANAN_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "BANAN_DISTRIBUTION"
		runtime "Release"
		optimize "on"