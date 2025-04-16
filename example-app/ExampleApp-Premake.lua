project "ExampleApp"
    location "./"
    targetname "%{prj.name}-bin"
    kind "ConsoleApp"
    staticruntime "off"

    language "C++"
    cppdialect "C++20"
    systemversion "latest"

    pchheader "Pch.hpp"
    pchsource "src/Pch.cpp"

    files {
        "src/**.hpp",
        "src/**.cpp"
    }

    includedirs {
        "src/",
        "../natrium-cpp/include/",
        "../natrium-cpp/%{IncludeDirectories.fmt}"
    }
    links {
        "NatriumCore",
        "%{Libraries.fmt}",
    }

    filter "system:linux"
        defines { "NA_PLATFORM_LINUX" }

    filter "system:windows"
        defines {
            "NA_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        }

        buildoptions {
            "/utf-8"
        }

    filter "configurations:dbg"
        symbols "On"
        runtime "Debug"
        defines { "NA_CONFIG_DEBUG" }

    filter "configurations:rel"
        optimize "speed"
        symbols "off"
        defines { "NA_CONFIG_RELEASE" }

    filter "configurations:dist"
        kind "WindowedApp"
        optimize "speed"
        symbols "off"
        defines { "NA_CONFIG_DIST" }
