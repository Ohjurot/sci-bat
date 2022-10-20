-- Add modules from a folder
function reti_module_folder(folder, group)
    for dir in io.popen("dir \"" .. folder .. "\" /b /ad"):lines() 
    do 
        reti_parse_module(group, folder .. "/" .. dir .. "/" .. "build.lua") 
    end
end

-- Parse a single module 
function reti_parse_module(grp, file)
    group(grp)
    include(file)
end

-- Conan no link setup (because visual studio will throw plenty of warnings at you)
function reti_conan_basic_setup()
    configurations{conan_build_type}
    architecture(conan_arch)
    includedirs{conan_includedirs}
    libdirs{conan_libdirs}
    defines{conan_defines}
    bindirs{conan_bindirs}
end

-- Begin new premake project
function reti_new_project(projectName, path)
    project(projectName)
    location("%{wks.location}/" .. path)
    targetdir("%{wks.location}/build/%{cfg.architecture}-%{cfg.shortname}/bin/")
    objdir("%{wks.location}/build/%{cfg.architecture}-%{cfg.shortname}/obj/%{cfg.buildtarget.name}/")
    libdirs {"%{cfg.buildtarget.directory}"}
    includedirs {"%{prj.location}", "%{wks.location}/src", "%{wks.location}/src/lib"}
    files { 
        "%{prj.location}/**.lua",
        "%{prj.location}/**.h", "%{prj.location}/**.hpp", "%{prj.location}/**.hh", "%{prj.location}/**.hxx",
        "%{prj.location}/**.c", "%{prj.location}/**.cpp", "%{prj.location}/**.cc", "%{prj.location}/**.cxx", 
        "%{prj.location}/**.md", "%{prj.location}/**.txt",
    }

    -- Windows / Unix
    filter { "system:Windows" }
        defines {  string.upper(projectName) .. "_WINDOWNS", "NOMINMAX" }
    filter {}
    filter { "system:Unix" }
        defines {  string.upper(projectName) .. "_UNIX" }
    filter {}

    -- Debug / Release
    filter "configurations:Debug"
        defines {  string.upper(projectName) .. "_DEBUG" }
        symbols "On"
    filter {}
    filter "configurations:Release"
        defines {  string.upper(projectName) .. "_RELEASE" }
        optimize "On"
    filter {}
end

-- Mark current project as executable
function reti_executable()
    kind "ConsoleApp"

    -- Conan links
    links{conan_libs}
    links{conan_system_libs}
    links{conan_frameworks}
end

-- Mark current project as static lib
function reti_lib()
    kind "StaticLib"
end

-- Mark current project as header only
function reti_header_only()
    kind "None"
end

-- Set current projects cpp version
function reti_cpp(stdcpp)
    stdcpp = stdcpp or "C++20"
    language "C++"
    cppdialect(stdcpp)
end
