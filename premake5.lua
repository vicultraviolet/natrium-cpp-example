OUTPUT_DIR = "%{cfg.buildcfg}-%{cfg.system}"

workspace "NatriumExample"
    architecture "x64"
    configurations { "dbg", "rel", "dist" }
    startproject "Sandbox"

    targetdir "bin/%{OUTPUT_DIR}/%{prj.name}/"
    objdir "bin-int/%{OUTPUT_DIR}/%{prj.name}/"

    debugdir "%{wks.location}"

include "natrium-cpp/Natrium-Premake.lua"
include "src/Examples-Premake.lua"
