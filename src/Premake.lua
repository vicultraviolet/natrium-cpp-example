    location "../"
    targetname "%{prj.name}-bin"
    kind "ConsoleApp"
    staticruntime "off"

    language "C++"
    cppdialect "C++20"
    systemversion "latest"

    pchheader "Pch.hpp"
    pchsource "Pch.cpp"

    files {
        "**.hpp",
        "**.cpp"
    }

    includedirs {
        "./",
        "%{wks.location}/natrium-cpp/include/",
        "%{wks.location}/natrium-cpp/%{IncludeDirectories.fmt}",
        "%{wks.location}/natrium-cpp/%{IncludeDirectories.glm}",
        "%{wks.location}/natrium-cpp/%{IncludeDirectories.imgui}",
        "%{wks.location}/natrium-cpp/%{IncludeDirectories.nlohmann_json}",
        "%{wks.location}/natrium-cpp/%{IncludeDirectories.stduuid}",
        "%{wks.location}/natrium-cpp/dependencies/"
    }
    links {
        "Natrium",
        "%{Libraries.tiny_obj_loader}",
        "%{Libraries.stb}",
        "%{Libraries.imgui}",
        "%{Libraries.glfw}",
        "%{Libraries.fmt}"
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