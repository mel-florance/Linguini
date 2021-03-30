workspace "Linguini"
	architecture "x64"
	startproject "Server"

	configurations {
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Server"
	kind "ConsoleApp"
	location "Server"
	language "C++"
 
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	disablewarnings {
		"4996",
		"4267",
		"4244",
		"4099",
	}

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/project/controllers/**.h"
	}

	includedirs {
		"Server/vendors",
		"Server/vendors/mysql/include",
		"Server/vendors/openssl/include",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands {
			"{COPY} ./project ../bin/Release-windows-x86_64/Server/project",
			"{COPY} ./project ../bin/Debug-windows-x86_64/Server/project",
			"{COPY} ./vendors/mysql/lib/libmysql.dll ../bin/Debug-windows-x86_64/Server",
			"{COPY} ./vendors/mysql/lib/libmysql.dll ../bin/Release-windows-x86_64/Server"
		}

		defines {
			"PLATFORM_WINDOWS",
			"_CRT_SECURE_NO_WARNINGS",
		}

		libdirs {
			"Server/vendors/mysql/lib",
			"Server/vendors/openssl/lib",
		}

		links {
			"libmysql",
			"libcrypto",
			"openssl",
			"libssl",
		}

	filter "system:linux"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands {
			"{COPY} ./project ../bin/Release-windows-x86_64/Server/project",
			"{COPY} ./project ../bin/Debug-windows-x86_64/Server/project",
		}

		defines {
			"PLATFORM_LINUX",
			"_CRT_SECURE_NO_WARNINGS",
			"_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING"
		}

		links {
			"mysql",
			"openssl",
		}

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"
		
	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "On"
