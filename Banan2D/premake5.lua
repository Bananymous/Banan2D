project "Banan2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "bgepch.h"
	pchsource "src/bgepch.cpp"

	if os.host() == "windows" then
		files
		{
			"src/*.h",
			"src/*.cpp",
			"src/Banan/**.h",
			"src/Banan/**.cpp",
			"src/Platform/OpenGL/**.h",
			"src/Platform/OpenGL/**.cpp",
			"src/Platform/Windows/**.h",
			"src/Platform/Windows/**.cpp",
			"vendor/stb_image/**.h",
			"vendor/stb_image/**.cpp",
			"vendor/glm/glm/**.hpp",
			"vendor/glm/glm/**.inl"
		}
	elseif os.host() == "linux" then
		files
		{
			"src/*.h",
			"src/*.cpp",
			"src/Banan/**.h",
			"src/Banan/**.cpp",
			"src/Platform/OpenGL/**.h",
			"src/Platform/OpenGL/**.cpp",
			"src/Platform/Linux/**.h",
			"src/Platform/Linux/**.cpp",
			"vendor/stb_image/**.h",
			"vendor/stb_image/**.cpp",
			"vendor/glm/glm/**.hpp",
			"vendor/glm/glm/**.inl"
		}
	end

	includedirs
	{
		"src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.glm}"
	}

	if os.host() == "windows" then
		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
		}
		links
		{
			"Glad",
			"imgui"
		}
	elseif os.host() == "linux" then
		links
		{
			"Glad",
			"imgui"
		}
	end

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
