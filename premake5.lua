workspace "OpenGL" 
    configurations { "Debug","Release" } 

project "OpenGL"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"

    files 
    { 
        "src/**.h",
        "src/**.cpp",
        "src/**.c",
        "src/**.hpp"
    }

    architecture "x64"
    cppdialect "C++20"

    includedirs 
    {
        "src",
        "vendor/include"
    }

    libdirs
    {
        "vendor/lib"
    }

    links 
    {
        "glfw3",
        "opengl32"
    }

    defines
    {

    }

    filter "configurations:Debug" 
        defines { "DEBUG" } 
        symbols "On"

    filter "configurations:Release" 
        defines { "NDEBUG" }
        optimize "On"
