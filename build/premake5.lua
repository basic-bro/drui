local libdir        = "../../../lib/"
local liblinkdir    = libdir .. "link/"
local libincludedir = libdir .. "include/"

local internallibdir = "../src"

-- local debug = "On"
local debug = "Off"

workspace( "drui" )
  location( "." )
  configurations( { "auto" } )
  system( "Windows" )
  architecture( "x86_64" )
  targetdir( "bin" )
  includedirs( { "../src", libincludedir } )
  libdirs({
    liblinkdir .. "SDL2",
    internallibdir .. "Layout",
    internallibdir .. "SDL_FontCache"
   })

  project( "SDL_FontCache" )
    location( "SDL_FontCache" )
    kind( "StaticLib" )
    language( "C++" )
    cppdialect( "C++20" )
    targetname( "SDL_FontCache" )
    symbols( debug )
    files( { "../src/SDL_FontCache/**.c" } )
    links( { } )

  project( "Layout" )
    location( "Layout" )
    kind( "StaticLib" )
    language( "C++" )
    cppdialect( "C++20" )
    targetname( "Layout" )
    symbols( debug )
    files( { "../src/Layout/**.cpp" } )
    links( { } )
    
  project( "app" )
    location( "app" )
    kind( "ConsoleApp" )
    language( "C++" )
    cppdialect( "C++20" )
    targetname( "app" )
    symbols( debug )
    files( { "../src/core/**.cpp", "../src/user/**.cpp" } )
    links( { "SDL2", "SDL2_image", "SDL2_ttf", "SDL2_gfx", "Layout", "SDL_FontCache" } )

