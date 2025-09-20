project "TextureExample"
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
        "%{NatriumDir}include/",
        "%{NatriumDir}%{IncludeDirectories.fmt}",
        "%{NatriumDir}%{IncludeDirectories.glm}",
        "%{NatriumDir}%{IncludeDirectories.imgui}",
        "%{NatriumDir}%{IncludeDirectories.nlohmann_json}",
        "%{NatriumDir}%{IncludeDirectories.stduuid}",
        "%{NatriumDir}dependencies/"
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
            "shaderc_combined",
            "%{Libraries.openal}",
            "%{Libraries.libsndfile}"
        }

        defines { "NA_PLATFORM_LINUX" }

    filter "system:windows"
        includedirs {
            "%{IncludeDirectories.vk}",
            "%{NatriumDir}%{IncludeDirectories.libsndfile}",
            "%{NatriumDir}%{IncludeDirectories.openal}" 
        }

        libdirs {
            "%{LibraryDirectories.vk}",
            "%{NatriumDir}%{LibraryDirectories.libsndfile}",
            "%{NatriumDir}%{LibraryDirectories.openal}" 
        }

        links {
            "vulkan-1"
        }

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
