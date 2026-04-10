# `CMakePresets.json` v2 requires CMake v3.20+.
# - https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
cmake_minimum_required(VERSION 3.22)

###
# TODO: Fix this doc up after make all cmake changes.
#
# Configurable params w/ defaults:
#   -DEKO_RENDERER=GL
#   -DEKO_LINUXDEPLOY_ARCH=x86_64          # See: cmake/appimage.cmake.in
#   -DEKO_PKG_SUFFIX=""
#   -DEKO_PKG_MACOS_USE_BUILD_UNI_DIR=OFF  # See: .github/workflows/macos.yml
#
# Configuring build:
#   # Super slow the first time as vcpkg downloads deps.
#   # This also downloads & extracts additional Assets to the `assets/` dir.
#   cmake --preset default
#
# Building:
#   cmake --build --preset default --config Release
#   cmake --build --preset default --config Debug
#
# Running:
#   cmake --build --preset default --config Release --target run
#   cmake --build --preset default --config Debug --target run
#   # Or directly:
#   ./bin/Release/EkoScape
#   ./bin/Debug/EkoScape
#
# Checking code quality (`cppcheck`):
#   cmake --build --preset default --config Release --target check
#
# Building Linux AppImage:
#   cmake --build --preset default --config Release --target appimage
#   # Test running it:
#   ./build/AppImage/EkoScape-x86_64.AppImage
#
# Packaging up files (CPack):
#   cmake --build --preset default --config Release --target package
#
# @author Bradley Whited
###
project("EkoScape"
    VERSION "2.4"
    HOMEPAGE_URL "https://github.com/esotericpig/EkoScape"
    LANGUAGES C CXX
)

#===========================================
# User Config
#===========================================
set(EKO_EXE_DIR_NAME "bin" CACHE STRING "Dir name to output the exe file to.")

# TODO: Rename to APPIMAGE?
# - https://github.com/linuxdeploy/linuxdeploy/releases/tag/continuous
set(EKO_LINUXDEPLOY_ARCH "x86_64" CACHE STRING "Arch of `linuxdeploy-<arch>.AppImage` for target 'appimage'.")
set_property(CACHE EKO_LINUXDEPLOY_ARCH PROPERTY STRINGS
    "aarch64" "armhf" "i386" "x86_64"
)

# TODO: Rename to CPACK?
set(EKO_PKG_SUFFIX "" CACHE STRING "Suffix of package name for CPack.")
option(EKO_PKG_MACOS_USE_BUILD_UNI_DIR "For macOS, use dir 'build_uni' for target 'package'." OFF)

message("
[EkoScape::UserConfig]
  EKO_EXE_DIR_NAME:                '${EKO_EXE_DIR_NAME}'

  EKO_LINUXDEPLOY_ARCH:            '${EKO_LINUXDEPLOY_ARCH}'

  EKO_PKG_SUFFIX:                  '${EKO_PKG_SUFFIX}'
  EKO_PKG_MACOS_USE_BUILD_UNI_DIR? ${EKO_PKG_MACOS_USE_BUILD_UNI_DIR}
")

#===========================================
# Project Config
#===========================================
set(RDNS_NAME "io.github.esotericpig.ekoscape")
set(BIN_NAME "${PROJECT_NAME}")

set(DOWNLOADS_DIR "${CMAKE_BINARY_DIR}/downloads")
set(ASSETS_NAME "assets")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/${ASSETS_NAME}")
set(RES_DIR "${CMAKE_SOURCE_DIR}/res")

set(CONFIG_CMAKE_IN_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(CONFIG_OUT_DIR "${CMAKE_BINARY_DIR}/config")
set(APPIMG_DIR "${CMAKE_BINARY_DIR}/AppImage")

#===========================================
# Download Assets
#===========================================
include("${CONFIG_CMAKE_IN_DIR}/DownloadAssets.cmake")

#===========================================
# Project Setup
#===========================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${EKO_EXE_DIR_NAME}/$<CONFIG>")

if(WIN32)
  # Statically link system libs.
  # - NOTE: Must set before add_executable()/add_library().
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(EkoScape WIN32)
add_library(EkoScapeBuildConfig INTERFACE)
add_library(EkoScape::BuildConfig ALIAS EkoScapeBuildConfig)

target_link_libraries(EkoScape PRIVATE EkoScape::BuildConfig)

#===========================================
# Build Setup
#===========================================
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

  target_compile_options(EkoScapeBuildConfig INTERFACE
      /utf-8
#      /W4 /permissive-
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

  target_compile_options(EkoScapeBuildConfig INTERFACE
      "-finput-charset=UTF-8"

      $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Emscripten>>:
        -Wall -Wconversion -Wextra -Wpedantic -Wsign-conversion -Wundef
        -fno-common
      >
  )
endif()

if(EMSCRIPTEN)
  include("${CONFIG_CMAKE_IN_DIR}/WebBuildConfig.cmake")
else()
  if(CYBEL_RENDERER STREQUAL "GL")
    target_compile_definitions(EkoScapeBuildConfig INTERFACE DANTARES_RENDERER_GL)
  endif()
endif()

#===========================================
# Libraries
#===========================================
set(FETCHCONTENT_QUIET OFF)

add_subdirectory("libs/CybelEngine")
add_subdirectory("libs/Dantares")

target_link_libraries(CybelEngine PRIVATE EkoScape::BuildConfig)
target_link_libraries(Dantares PRIVATE EkoScape::BuildConfig)

target_link_libraries(EkoScape PRIVATE
    CybelEngine::CybelEngine
    Dantares::Dantares
)

#===========================================
# Source Code
#===========================================
target_include_directories(EkoScape PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)
# Run `./scripts/glob_src.rb` to show source files to add/remove here.
target_sources(EkoScape PRIVATE
    "$<$<BOOL:${WIN32}>:${RES_DIR}/res.rc>"

    # @glob_src=src:src/
    "src/ekoscape_game.cpp"
    "src/main.cpp"
    "src/assets/assets.cpp"
    "src/assets/font_renderer.cpp"
    "src/map/dantares_map.cpp"
    "src/map/facing.cpp"
    "src/map/map.cpp"
    "src/map/map_grid.cpp"
    "src/map/space.cpp"
    "src/map/space_type.cpp"
    "src/scenes/boring_work_scene.cpp"
    "src/scenes/dantares_renderer.cpp"
    "src/scenes/game_hud.cpp"
    "src/scenes/game_overlay.cpp"
    "src/scenes/game_scene.cpp"
    "src/scenes/menu_credits_scene.cpp"
    "src/scenes/menu_play_scene.cpp"
    "src/scenes/menu_scene.cpp"
    "src/scenes/scene_action.cpp"
    "src/world/robot.cpp"
    "src/world/star_sys.cpp"
    # @glob_src
)

#===========================================
# Custom Targets
#===========================================
include("${CONFIG_CMAKE_IN_DIR}/WorkflowTargets.cmake")

#===========================================
# Dist. Setup
#===========================================
include("${CONFIG_CMAKE_IN_DIR}/AppImageConfig.cmake")
include("${CONFIG_CMAKE_IN_DIR}/ItchioConfig.cmake")
include("${CONFIG_CMAKE_IN_DIR}/CPackConfig.cmake")
