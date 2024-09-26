# 'CMakePresets.json' v2 must at least have CMake v3.20.
# - https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
cmake_minimum_required(VERSION 3.20)

###
# Setup:
#   ## Install deps w/ vcpkg (super slow the first time as it downloads deps).
#   $ cmake --preset=default
#
# Building:
#   $ cmake --build --preset=default --config Release
#   $ cmake --build --preset=default --config Debug
#
# Running:
#   $ cmake --build --preset=default --config Release -t run
#   $ cmake --build --preset=default --config Debug -t run
#   ## Or directly:
#   $ ./bin/Release/EkoScape
#   $ ./bin/Debug/EkoScape
#
# Checking Code Quality:
#   $ cmake --build --preset=default -t check
###

set(PROJ_NAME "EkoScape")
set(BIN_NAME "${PROJ_NAME}")
set(BIN_DIRNAME "bin")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(TP_DIR "${CMAKE_SOURCE_DIR}/third_party")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${BIN_DIRNAME}/$<CONFIG>")

project("${PROJ_NAME}" LANGUAGES CXX)
add_executable("${BIN_NAME}")

# Also defined in 'CMakePresets.json' because sometimes doesn't work otherwise (for some reason).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")

  # For Windows, don't really care about warnings.
  #target_compile_options("${BIN_NAME}" PRIVATE /W4 /permissive-)
else() #elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

  target_compile_options("${BIN_NAME}" PRIVATE -Wall -Wconversion -Wextra -Wpedantic)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_compile_definitions("${BIN_NAME}" PRIVATE CYBEL_MACOS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_definitions("${BIN_NAME}" PRIVATE CYBEL_WINDOWS)
else() #elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_definitions("${BIN_NAME}" PRIVATE CYBEL_LINUX)
endif()

# Might be needed if get warnings.
#set(OpenGL_GL_PREFERENCE LEGACY)

# See 'vcpkg.json' & output of CMake for usage.
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED) # GLU should be included.
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(tinyutf8 CONFIG REQUIRED)

target_link_libraries("${BIN_NAME}" PRIVATE
    GLEW::GLEW
    OpenGL::GL
    OpenGL::GLU
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    tinyutf8::tinyutf8
)

target_include_directories("${BIN_NAME}" PRIVATE
    "${TP_DIR}"
    "${SRC_DIR}"
)
target_sources("${BIN_NAME}" PRIVATE
    ${TP_DIR}/Dantares/Dantares.cpp

    ${SRC_DIR}/cybel/audio/music.cpp
    ${SRC_DIR}/cybel/gfx/font_atlas.cpp
    ${SRC_DIR}/cybel/gfx/image.cpp
    ${SRC_DIR}/cybel/gfx/sprite.cpp
    ${SRC_DIR}/cybel/gfx/sprite_atlas.cpp
    ${SRC_DIR}/cybel/gfx/texture.cpp
    ${SRC_DIR}/cybel/io/text_reader.cpp
    ${SRC_DIR}/cybel/io/text_reader_buf.cpp
    ${SRC_DIR}/cybel/render/renderer.cpp
    ${SRC_DIR}/cybel/scene/scene_bag.cpp
    ${SRC_DIR}/cybel/scene/scene_man.cpp
    ${SRC_DIR}/cybel/util/cybel_error.cpp
    ${SRC_DIR}/cybel/util/duration.cpp
    ${SRC_DIR}/cybel/util/rando.cpp
    ${SRC_DIR}/cybel/util/timer.cpp
    ${SRC_DIR}/cybel/util/util.cpp
    ${SRC_DIR}/cybel/vfx/particle.cpp
    ${SRC_DIR}/cybel/cybel_engine.cpp
    ${SRC_DIR}/cybel/types.cpp

    ${SRC_DIR}/assets/assets.cpp
    ${SRC_DIR}/assets/font_renderer.cpp
    ${SRC_DIR}/assets/styled_graphics.cpp

    ${SRC_DIR}/map/dantares_map.cpp
    ${SRC_DIR}/map/facing.cpp
    ${SRC_DIR}/map/map.cpp
    ${SRC_DIR}/map/map_grid.cpp
    ${SRC_DIR}/map/space.cpp
    ${SRC_DIR}/map/space_type.cpp

    ${SRC_DIR}/scenes/boring_work_scene.cpp
    ${SRC_DIR}/scenes/game_hud.cpp
    ${SRC_DIR}/scenes/game_overlay.cpp
    ${SRC_DIR}/scenes/game_scene.cpp
    ${SRC_DIR}/scenes/menu_credits_scene.cpp
    ${SRC_DIR}/scenes/menu_play_scene.cpp
    ${SRC_DIR}/scenes/menu_scene.cpp

    ${SRC_DIR}/world/robot.cpp
    ${SRC_DIR}/world/star_sys.cpp

    ${SRC_DIR}/ekoscape_game.cpp
    ${SRC_DIR}/main.cpp
)

add_custom_target(run
    COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BIN_NAME}"
    DEPENDS "${BIN_NAME}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    USES_TERMINAL # For terminal output during testing.
    VERBATIM
)
add_custom_target(check
    COMMAND cppcheck --quiet --enable=all
              --language=c++ "--std=c++${CMAKE_CXX_STANDARD}"
              "--relative-paths=${CMAKE_SOURCE_DIR}"

              --suppress=missingInclude
              "--suppress=noExplicitConstructor:src/cybel/types.h"
              --suppress=unusedFunction
              --suppress=unusedPrivateFunction

              "${TP_DIR}"
              "${SRC_DIR}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)
