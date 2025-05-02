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
        "../natrium-cpp/%{IncludeDirectories.fmt}",
        "../natrium-cpp/%{IncludeDirectories.glm}",
    }
    links {
        "Natrium",
        "%{Libraries.tiny_obj_loader}",
        "%{Libraries.stb}",
        "%{Libraries.glfw}",
        "%{Libraries.fmt}",
    }

    filter "system:linux"
        links {
            "vulkan",
            "shaderc_combined"
        }

        defines { "NA_PLATFORM_LINUX" }

    filter "system:windows"
        includedirs "%{IncludeDirectories.vk}" 
		libdirs "%{LibraryDirectories.vk}" 

        links "vulkan-1"

        defines {
            "NA_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        }

        buildoptions { "/utf-8" }

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

    filter { "system:windows", "configurations:dbg" }
        links "shaderc_combinedd"
    filter { "system:windows", "configurations:rel or dist" }
        links "shaderc_combined"