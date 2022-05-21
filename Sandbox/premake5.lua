project "Sandbox"
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


	if os.host() == "windows" then
		links
		{
			"Banan2D",
			"GLFW",
			"Glad",
			"imgui",
			"opengl32.lib",
			"XInput.lib"
		}
	elseif os.host() == "linux" then
		links
		{
			"Banan2D",
			"Glad",
			"imgui",
			"X11",
			"dl"
		}
	end

	filter "options:glfw"
		defines "BANAN_USE_GLFW"

	filter { "system:windows", "options:glfw" }
		kind "ConsoleApp"
		systemversion "latest"

	filter { "system:windows", "not options:glfw" }
		kind "WindowedApp"
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