cmake_minimum_required(VERSION 3.24)
project(game_engine VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---- Lua 5.4 (compiled from vendored source) ----
file(GLOB LUA_SOURCES thirdparty/lua/*.c)
list(REMOVE_ITEM LUA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lua/lua.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lua/luac.c"
)
add_library(lua_static STATIC ${LUA_SOURCES})
target_include_directories(lua_static PUBLIC thirdparty/lua)
set_target_properties(lua_static PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
if(NINTENDO_SWITCH)
  add_compile_definitions(LLONG_MAX=__LONG_LONG_MAX__)
endif()

# ---- Box2D 2.4.1 (compiled from vendored source) ----
file(GLOB_RECURSE BOX2D_SOURCES thirdparty/box2d-2.4.1/src/*.cpp)
add_library(box2d STATIC ${BOX2D_SOURCES})
target_include_directories(box2d PUBLIC thirdparty/box2d-2.4.1/include)
target_include_directories(box2d PRIVATE thirdparty/box2d-2.4.1/src)

# ---- SDL2 family (system package manager) ----
# macOS:  brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer
# Linux:  apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
# Windows: Fetched automatically
if(WIN32)
  include(FetchContent)
  FetchContent_Declare(sdl2_vc URL https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-VC.zip)
  FetchContent_Declare(sdl2_image_vc URL https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-devel-2.6.3-VC.zip)
  FetchContent_Declare(sdl2_ttf_vc URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/SDL2_ttf-devel-2.20.2-VC.zip)
  FetchContent_Declare(sdl2_mixer_vc URL https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-devel-2.6.3-VC.zip)
  FetchContent_MakeAvailable(sdl2_vc sdl2_image_vc sdl2_ttf_vc sdl2_mixer_vc)

  add_library(SDL2::SDL2main STATIC IMPORTED)
  set_target_properties(SDL2::SDL2main PROPERTIES IMPORTED_LOCATION "${sdl2_vc_SOURCE_DIR}/lib/x64/SDL2main.lib")

  add_library(SDL2::SDL2 SHARED IMPORTED)
  set_target_properties(SDL2::SDL2 PROPERTIES
    IMPORTED_IMPLIB "${sdl2_vc_SOURCE_DIR}/lib/x64/SDL2.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${sdl2_vc_SOURCE_DIR}/include"
  )

  add_library(SDL2_image::SDL2_image SHARED IMPORTED)
  set_target_properties(SDL2_image::SDL2_image PROPERTIES
    IMPORTED_IMPLIB "${sdl2_image_vc_SOURCE_DIR}/lib/x64/SDL2_image.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${sdl2_image_vc_SOURCE_DIR}/include"
  )

  add_library(SDL2_ttf::SDL2_ttf SHARED IMPORTED)
  set_target_properties(SDL2_ttf::SDL2_ttf PROPERTIES
    IMPORTED_IMPLIB "${sdl2_ttf_vc_SOURCE_DIR}/lib/x64/SDL2_ttf.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${sdl2_ttf_vc_SOURCE_DIR}/include"
  )

  add_library(SDL2_mixer::SDL2_mixer SHARED IMPORTED)
  set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
    IMPORTED_IMPLIB "${sdl2_mixer_vc_SOURCE_DIR}/lib/x64/SDL2_mixer.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${sdl2_mixer_vc_SOURCE_DIR}/include"
  )
elseif(NINTENDO_SWITCH)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2       REQUIRED IMPORTED_TARGET sdl2)
  pkg_check_modules(SDL2_image REQUIRED IMPORTED_TARGET SDL2_image)
  pkg_check_modules(SDL2_ttf   REQUIRED IMPORTED_TARGET SDL2_ttf)
  pkg_check_modules(SDL2_mixer REQUIRED IMPORTED_TARGET SDL2_mixer)
  add_library(SDL2::SDL2             ALIAS PkgConfig::SDL2)
  add_library(SDL2_image::SDL2_image ALIAS PkgConfig::SDL2_image)
  add_library(SDL2_ttf::SDL2_ttf     ALIAS PkgConfig::SDL2_ttf)
  add_library(SDL2_mixer::SDL2_mixer ALIAS PkgConfig::SDL2_mixer)
else()
  find_package(SDL2       REQUIRED)
  find_package(SDL2_image REQUIRED)
  find_package(SDL2_ttf   REQUIRED)
  find_package(SDL2_mixer REQUIRED)
endif()

# ---- Game engine executable ----
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)
add_executable(game_engine ${ENGINE_SOURCES})
set_target_properties(game_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}"
)

target_include_directories(game_engine PRIVATE src)
# SYSTEM suppresses warnings from thirdparty headers (Helper.h, LuaBridge, etc.)
target_include_directories(game_engine SYSTEM PRIVATE
    thirdparty
    thirdparty/box2d-2.4.1/include
)

target_compile_definitions(game_engine PRIVATE
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)

if(MSVC)
  target_compile_options(game_engine PRIVATE
        /W4
        $<$<CONFIG:Release>:/Ox /arch:AVX2>
    )
  target_link_options(game_engine PRIVATE
        $<$<CONFIG:Release>:/LTCG /PROFILE>
    )
else()
  target_compile_options(game_engine PRIVATE
        -Wall -Wextra -Werror
        -Wno-deprecated-declarations
        -Wno-unused-function
    )
  if(APPLE)
    target_compile_options(game_engine PRIVATE -Wno-nontrivial-memcall)
  elseif(NINTENDO_SWITCH)
    target_compile_options(game_engine PRIVATE -Wno-template-body)
  endif()
  target_compile_options(game_engine PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:Clang,AppleClang>>:-flto=full>
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>>:-flto>
        $<$<CONFIG:Release>:-g3>
        $<$<CONFIG:Debug>:-g3>
        $<$<CONFIG:Debug>:-fsanitize=undefined>
        $<$<CONFIG:Debug>:-pg>
    )
  target_link_options(game_engine PRIVATE
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:Clang,AppleClang>>:-flto=full>
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU>>:-flto>
        $<$<CONFIG:Debug>:-fsanitize=undefined>
        $<$<CONFIG:Debug>:-pg>
    )
endif()

target_link_libraries(game_engine PRIVATE
    box2d
    lua_static
)
if(WIN32)
  target_link_libraries(game_engine PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(game_engine PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
)

if(NINTENDO_SWITCH)
  nx_create_nro(game_engine ROMFS "${CMAKE_SOURCE_DIR}/resources")
  add_custom_target(game_engine_deploy ALL
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_BINARY_DIR}/game_engine.nro"
        "${CMAKE_SOURCE_DIR}/game_engine.nro"
    COMMAND ${CMAKE_COMMAND} -E rm -f
        "${CMAKE_SOURCE_DIR}/game_engine.elf"
    DEPENDS game_engine_nro
  )
endif()

if(WIN32)
  add_custom_command(TARGET game_engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${sdl2_vc_SOURCE_DIR}/lib/x64" $<TARGET_FILE_DIR:game_engine>
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${sdl2_image_vc_SOURCE_DIR}/lib/x64" $<TARGET_FILE_DIR:game_engine>
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${sdl2_ttf_vc_SOURCE_DIR}/lib/x64" $<TARGET_FILE_DIR:game_engine>
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${sdl2_mixer_vc_SOURCE_DIR}/lib/x64" $<TARGET_FILE_DIR:game_engine>
    )
endif()
