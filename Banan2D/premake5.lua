project "Banan2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "bgepch.h"
	pchsource "src/bgepch.cpp"



	-- PROJECT FILES

	files {
		"src/*.h",
		"src/*.cpp",
		"src/Banan/**.h",
		"src/Banan/**.cpp",
		"src/Platform/OpenGL/**.h",
		"src/Platform/OpenGL/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl"
	}



	-- INCLUDE DIRECTORIES

	includedirs {
		"src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.glm}"
	}
	


	-- PROJECT FILES

	filter { "options:glfw" }
		files {
			"src/Platform/GLFW/**.h",
			"src/Platform/GLFW/**.cpp"
		}

	filter { "system:linux" }
		files {
			"src/Platform/Linux/**.h",
			"src/Platform/Linux/**.cpp"
		}

	filter { "system:windows" }
		files {
			"src/Platform/Windows/**.h",
			"src/Platform/Windows/**.cpp"
		}

	filter { "system:linux", "options:glfw" }
		removefiles {
			"src/Platform/Linux/LinuxInput.*",
			"src/Platform/Linux/LinuxOpenGLContext.*",
			"src/Platform/Linux/LinuxWindow.*"
		}

	filter { "system:windows", "options:glfw" }
		removefiles {
			"src/Platform/Windows/WindowsImGuiLayer.*",
			"src/Platform/Windows/WindowsInput.*",
			"src/Platform/Windows/OpenGLContext.*",
			"src/Platform/Windows/WindowsWindows.*"
		}



	-- PLATFORM SPECIFIC

	filter "options:glfw"
		defines "BANAN_USE_GLFW"

	filter "system:windows"
		defines "_CRT_SECURE_NO_WARNINGS"
		systemversion "latest"



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
