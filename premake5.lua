-- Project SCI-Energiespeicher root premake5 configuration
-- This will generate the projected to be compiled by the compiler

include "conanbuildinfo.premake.lua"
include "scripts/premake-utils.lua"

-- Begin basic configuration
workspace "SCI-Energiespeicher" 
linkgroups "On"
reti_conan_basic_setup()

-- Add source packages
reti_module_folder("src/lib", "Libraries")
reti_module_folder("src/app", "Application")
reti_module_folder("src/example", "Examples")
reti_module_folder("src/test", "Unittests")

