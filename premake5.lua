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
 
	targetdir ("Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("Build/" .. outputdir .. "/%{prj.name}")

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

		defines {
			"PLATFORM_LINUX",
			"_CRT_SECURE_NO_WARNINGS",
			"_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING"
		}

		links {
			"mysqlclient:static",
			"mysqlcppconn-static:static",
			"ssl",
			"crypto",
			"pthread",
			"dl",
			"z"
		}

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"

		if os.host() == "linux" then
			postbuildcommands {
				"{COPY} ./project ../Binaries/Debug-linux-x86_64/Server/project",
			}
		end

		if os.host() == "windows" then
			postbuildcommands {
				"{COPY} ./project ../Binaries/Debug-windows-x86_64/Server/project",
			}
		end
		
	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "On"

		if os.host() == "linux" then
			postbuildcommands {
				"{COPY} ./project ../Binaries/Release-linux-x86_64/Server/project",
			}
		end

		if os.host() == "windows" then
			postbuildcommands {
				"{COPY} ./project ../Binaries/Release-windows-x86_64/Server/project",
			}
		end
