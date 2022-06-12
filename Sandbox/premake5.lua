project "Sandbox"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp"
	}

	includedirs {
		"%{wks.location}/Banan2D/src",
		"%{wks.location}/Banan2D/vendor",
		"%{IncludeDir.glm}"
	}


	-- LINKS

	links { "Banan2D", "Glad", "imgui" }

	filter { "options:glfw" }
		links { "GLFW" }

	filter { "system:windows", "options:glfw" }
		links { "Ws2_32.lib" }

	filter { "system:windows", "not options:glfw" }
		links { "opengl32.lib", "XInput.lib", "Ws2_32.lib" }

	filter { "system:linux", "options:glfw" }
		links { "pthread", "X11", "dl" }



	-- PLATFORM SPECIFIC

	filter { "action:vs*" }
		flags "NoImplicitLink"

	filter { "system:windows", "options:glfw" }
		kind "ConsoleApp"
		systemversion "latest"

	filter { "system:windows", "not options:glfw" }
		kind "WindowedApp"
		systemversion "latest"

	filter { "system:linux", "options:glfw" }
		kind "WindowedApp"
		systemversion "latest"

	filter "options:glfw"
		defines "BANAN_USE_GLFW"


	-- CONFIGURATIONS

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
