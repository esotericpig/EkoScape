# 'CMakePresets.json' v2 must at least have CMake v3.20.
# - https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
cmake_minimum_required(VERSION 3.20)

###
# Check out the `Hacking` section in 'README.md' for how to build, etc.
#
# @author Bradley Whited
###

#message(STATUS "Using toolchain file: '${CMAKE_TOOLCHAIN_FILE}'.") # Gets rid of stupid warning.

project("EkoScape"
    LANGUAGES C CXX
)

set(RDNS_NAME "com.github.esotericpig.${PROJECT_NAME}")
set(BIN_NAME "${PROJECT_NAME}")
set(BIN_DIRNAME "bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${BIN_DIRNAME}/$<CONFIG>")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(TP_DIR "${CMAKE_SOURCE_DIR}/third_party")
set(DOWNLOADS_DIR "${CMAKE_BINARY_DIR}/downloads")
set(DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

set(ASSETS_NAME "assets")
set(ASSETS_FILENAME "${ASSETS_NAME}.zip")
set(ASSETS_URL "https://github.com/esotericpig/EkoScape/releases/download/v1.99/${ASSETS_FILENAME}")
set(ASSETS_DOWN_FILE "${DOWNLOADS_DIR}/${ASSETS_FILENAME}")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/${ASSETS_NAME}")

set(TEMPL_CMAKE_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(TEMPL_DIR "${CMAKE_BINARY_DIR}/templ")
set(LINUXDEPLOY_ARCH "x86_64" CACHE STRING "Arch of `linuxdeploy-<arch>.AppImage` for 'appimage' target.")

set(PKG_SUFFIX "" CACHE STRING "Suffix of package name for CPack.")
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_VERBATIM_VARIABLES ON)
set(CPACK_MONOLITHIC_INSTALL OFF)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL "cpack")
set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE")

set(FETCHCONTENT_QUIET OFF)
#set(OpenGL_GL_PREFERENCE LEGACY) # Might need if get warnings.

# Also defined in 'CMakePresets.json' because sometimes doesn't work otherwise (for some reason).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#include(FetchContent)

add_executable("${BIN_NAME}")

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O3 /DNDEBUG")

  # For Windows, don't really care about warnings.
  #target_compile_options("${BIN_NAME}" PRIVATE /W4 /permissive-)
else() #elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

  target_compile_options("${BIN_NAME}" PRIVATE -Wall -Wconversion -Wextra -Wpedantic)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_compile_definitions("${BIN_NAME}" PRIVATE CYBEL_MACOS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_definitions("${BIN_NAME}" PRIVATE CYBEL_WINDOWS)
else() #elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_definitions("${BIN_NAME}" PRIVATE CYBEL_LINUX)
endif()

if(NOT EXISTS "${ASSETS_DIR}/images/EkoScape.png")
  if(NOT EXISTS "${ASSETS_DOWN_FILE}")
    message(STATUS "Downloading '${ASSETS_URL}' to '${ASSETS_DOWN_FILE}'...")
    file(DOWNLOAD
        "${ASSETS_URL}"
        "${ASSETS_DOWN_FILE}"

        #EXPECTED_HASH SHA256=
        TLS_VERIFY ON
        SHOW_PROGRESS
    )
  endif()

  message(STATUS "Extracting '${ASSETS_DOWN_FILE}' to '${ASSETS_DIR}'...")
  file(ARCHIVE_EXTRACT
      INPUT "${ASSETS_DOWN_FILE}"
      DESTINATION "${ASSETS_DIR}"

      PATTERNS "icons" "images" "music" "textures"
      #LIST_ONLY # For testing.
      VERBOSE
  )
endif()

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
# Run `./scripts/glob_src.rb -u` to auto-update list of source files here.
target_sources("${BIN_NAME}" PRIVATE
    "${TP_DIR}/Dantares/Dantares.cpp"

    "${SRC_DIR}/cybel/audio/music.cpp"
    "${SRC_DIR}/cybel/gfx/font_atlas.cpp"
    "${SRC_DIR}/cybel/gfx/image.cpp"
    "${SRC_DIR}/cybel/gfx/sprite.cpp"
    "${SRC_DIR}/cybel/gfx/sprite_atlas.cpp"
    "${SRC_DIR}/cybel/gfx/texture.cpp"
    "${SRC_DIR}/cybel/io/text_reader.cpp"
    "${SRC_DIR}/cybel/io/text_reader_buf.cpp"
    "${SRC_DIR}/cybel/render/renderer.cpp"
    "${SRC_DIR}/cybel/scene/scene_bag.cpp"
    "${SRC_DIR}/cybel/scene/scene_man.cpp"
    "${SRC_DIR}/cybel/util/cybel_error.cpp"
    "${SRC_DIR}/cybel/util/duration.cpp"
    "${SRC_DIR}/cybel/util/rando.cpp"
    "${SRC_DIR}/cybel/util/timer.cpp"
    "${SRC_DIR}/cybel/util/util.cpp"
    "${SRC_DIR}/cybel/vfx/particle.cpp"
    "${SRC_DIR}/cybel/cybel_engine.cpp"
    "${SRC_DIR}/cybel/types.cpp"

    "${SRC_DIR}/assets/assets.cpp"
    "${SRC_DIR}/assets/font_renderer.cpp"
    "${SRC_DIR}/assets/styled_graphics.cpp"

    "${SRC_DIR}/map/dantares_map.cpp"
    "${SRC_DIR}/map/facing.cpp"
    "${SRC_DIR}/map/map.cpp"
    "${SRC_DIR}/map/map_grid.cpp"
    "${SRC_DIR}/map/space.cpp"
    "${SRC_DIR}/map/space_type.cpp"

    "${SRC_DIR}/scenes/boring_work_scene.cpp"
    "${SRC_DIR}/scenes/game_hud.cpp"
    "${SRC_DIR}/scenes/game_overlay.cpp"
    "${SRC_DIR}/scenes/game_scene.cpp"
    "${SRC_DIR}/scenes/menu_credits_scene.cpp"
    "${SRC_DIR}/scenes/menu_play_scene.cpp"
    "${SRC_DIR}/scenes/menu_scene.cpp"
    "${SRC_DIR}/scenes/scene_action.cpp"

    "${SRC_DIR}/world/robot.cpp"
    "${SRC_DIR}/world/star_sys.cpp"

    "${SRC_DIR}/ekoscape_game.cpp"
    "${SRC_DIR}/main.cpp"
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

file(GLOB TEMPL_CMAKE_FILES "${TEMPL_CMAKE_DIR}/*.cmake.in")
foreach(TEMPL_FILE ${TEMPL_CMAKE_FILES})
  cmake_path(GET TEMPL_FILE STEM TEMPL_NAME)
  configure_file("${TEMPL_FILE}" "${TEMPL_DIR}/${TEMPL_NAME}.cmake" @ONLY)

  add_custom_target("${TEMPL_NAME}"
      COMMAND "${CMAKE_COMMAND}" -P "${TEMPL_DIR}/${TEMPL_NAME}.cmake"
      DEPENDS "${BIN_NAME}"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      USES_TERMINAL
      VERBATIM
  )
endforeach()

install(TARGETS "${BIN_NAME}"
    RUNTIME DESTINATION bin
            COMPONENT appimage
            EXCLUDE_FROM_ALL
    ARCHIVE DESTINATION lib  # For static libs.
            COMPONENT appimage
            EXCLUDE_FROM_ALL
    LIBRARY DESTINATION lib  # For shared libs.
            COMPONENT appimage
            EXCLUDE_FROM_ALL
)
install(FILES "${DIST_DIR}/${RDNS_NAME}.desktop"
    DESTINATION "share/applications"
    COMPONENT appimage
    EXCLUDE_FROM_ALL
)
install(FILES "${DIST_DIR}/${RDNS_NAME}.appdata.xml"
    DESTINATION "share/metainfo"
    COMPONENT appimage
    EXCLUDE_FROM_ALL
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-linux")
  set(CPACK_GENERATOR "TGZ")

  file(GLOB APPIMAGE_FILES "${CMAKE_BINARY_DIR}/*.AppImage")
  install(PROGRAMS ${APPIMAGE_FILES}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
  install(FILES "${DIST_DIR}/${RDNS_NAME}.desktop"
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-macos")
  set(CPACK_GENERATOR "ZIP")

  install(TARGETS "${BIN_NAME}"
      RUNTIME DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-windows")
  set(CPACK_GENERATOR "ZIP")

  install(TARGETS "${BIN_NAME}"
      RUNTIME DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
endif()

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}${PKG_SUFFIX}")

install(DIRECTORY "${ASSETS_DIR}"
    DESTINATION "."
    COMPONENT cpack
    EXCLUDE_FROM_ALL
)
install(FILES
    "${CMAKE_SOURCE_DIR}/README.md"
    "${CMAKE_SOURCE_DIR}/LICENSE"
    DESTINATION "."
    COMPONENT cpack
    EXCLUDE_FROM_ALL
)

include(CPack)
