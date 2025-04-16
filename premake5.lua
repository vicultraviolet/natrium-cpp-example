OUTPUT_DIR = "%{cfg.buildcfg}-%{cfg.system}"

workspace "NatriumExample"
    architecture "x64"
    configurations { "dbg", "rel", "dist" }
    startproject "ExampleApp"

    targetdir "bin/%{OUTPUT_DIR}/%{prj.name}/"
    objdir "bin-int/%{OUTPUT_DIR}/%{prj.name}/"

include "natrium-cpp/Natrium-Premake.lua"
include "example-app/ExampleApp-Premake.lua"