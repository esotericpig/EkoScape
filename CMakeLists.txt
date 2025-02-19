# 'CMakePresets.json' v2 must at least have CMake v3.20.
# - https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
cmake_minimum_required(VERSION 3.20)

###
# Configurable params w/ defaults:
#   -DRENDERER=GL
#   -DLINUXDEPLOY_ARCH=x86_64      # See: cmake/appimage.cmake.in
#   -DPKG_SUFFIX=""
#   -DPKG_MACOS_BUILD_UNI_DIR=OFF  # See: .github/workflows/macos.yml
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

# Get rid of stupid warning on first run.
message(STATUS "Using toolchain file: '${CMAKE_TOOLCHAIN_FILE}'.")

project("EkoScape"
    VERSION "2.1"
    HOMEPAGE_URL "https://github.com/esotericpig/EkoScape"
    LANGUAGES C CXX
)

string(TOLOWER "${PROJECT_NAME}" PROJ_ID)
set(RDNS_NAME "io.github.esotericpig.${PROJ_ID}")
set(BIN_NAME "${PROJECT_NAME}")
set(BIN_DIRNAME "bin" CACHE STRING "Dirname to output the binary file to.")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${BIN_DIRNAME}/$<CONFIG>")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(TP_DIR "${CMAKE_SOURCE_DIR}/third_party")
set(RENDERER "GL" CACHE STRING "Renderer to use: GL or GLES.")
set(DOWNLOADS_DIR "${CMAKE_BINARY_DIR}/downloads")
set(RES_DIR "${CMAKE_SOURCE_DIR}/res")

set(ASSETS_NAME "assets")
set(ASSETS_FILENAME "${ASSETS_NAME}.zip")
set(ASSETS_URL "https://github.com/esotericpig/EkoScape/releases/download/v1.99/${ASSETS_FILENAME}")
set(ASSETS_DOWN_FILE "${DOWNLOADS_DIR}/${ASSETS_FILENAME}")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/${ASSETS_NAME}")

set(TEMPL_CMAKE_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(TEMPL_DIR "${CMAKE_BINARY_DIR}/templ")
set(APPIMG_DIR "${CMAKE_BINARY_DIR}/AppImage")
set(LINUXDEPLOY_ARCH "x86_64" CACHE STRING "Arch of `linuxdeploy-<arch>.AppImage` for 'appimage' target.")

set(PKG_SUFFIX "" CACHE STRING "Suffix of package name for CPack.")
option(PKG_MACOS_BUILD_UNI_DIR "For macOS, use 'build_uni' dir for 'package' target." OFF)
set(PKG_BASE_FILES "${TEMPL_DIR}/.itch.toml")
set(PKG_RES_DIRS "${ASSETS_DIR}")
set(PKG_RES_FILES
    "${CMAKE_SOURCE_DIR}/README.md"
    "${CMAKE_SOURCE_DIR}/LICENSE"
)

set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_VERBATIM_VARIABLES ON)
set(CPACK_MONOLITHIC_INSTALL OFF)
set(CPACK_COMPONENTS_ALL "cpack")
set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE")

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_BUNDLE_COMPONENT_INSTALL ON)

set(CPACK_BUNDLE_NAME "${BIN_NAME}")
set(CPACK_BUNDLE_PLIST "${TEMPL_DIR}/Info.plist")
set(CPACK_BUNDLE_ICON "${ASSETS_DIR}/icons/${PROJECT_NAME}.icns")

set(FETCHCONTENT_QUIET OFF)
#set(OpenGL_GL_PREFERENCE LEGACY) # Might need if get warnings.

if(APPLE)
  set(PKG_ITCH_PLAY_PATH "${BIN_NAME}.app")
  set(PKG_ITCH_RES_PATH "./${BIN_NAME}.app/Contents/Resources")
elseif(WIN32)
  set(PKG_ITCH_PLAY_PATH "${BIN_NAME}.exe")
  set(PKG_ITCH_RES_PATH ".")
else()
  set(PKG_ITCH_PLAY_PATH "${BIN_NAME}.sh")
  set(PKG_ITCH_RES_PATH ".")
endif()

############################################
# Compiler Config                          #
############################################
# Also defined in 'CMakePresets.json' because sometimes doesn't work otherwise (for some reason).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
  # Statically link system libs.
  # NOTE: Must be before add_executable().
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable("${BIN_NAME}")

if(EMSCRIPTEN)
  # TODO: Implement Emscripten code.
  message(FATAL_ERROR "Emscripten code not implemented yet.")
elseif(APPLE)
  target_compile_definitions("${BIN_NAME}" PRIVATE
      CYBEL_PLATFORM_MACOS
      DANTARES_PLATFORM_MACOS
  )
elseif(WIN32)
  set_target_properties("${BIN_NAME}" PROPERTIES
      WIN32_EXECUTABLE ON
  )
  target_compile_definitions("${BIN_NAME}" PRIVATE
      CYBEL_PLATFORM_WINDOWS
      DANTARES_PLATFORM_WINDOWS
  )
  target_sources("${BIN_NAME}" PRIVATE "${RES_DIR}/res.rc")
else()
  target_compile_definitions("${BIN_NAME}" PRIVATE
      CYBEL_PLATFORM_LINUX
      DANTARES_PLATFORM_LINUX
  )
endif()

if(RENDERER MATCHES "GLES")
  target_compile_definitions("${BIN_NAME}" PRIVATE
      CYBEL_RENDERER_GLES
  )
else() # GL.
  target_compile_definitions("${BIN_NAME}" PRIVATE
      CYBEL_RENDERER_GL
      DANTARES_RENDERER_GL
  )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG /utf-8")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /utf-8")

  #target_compile_options("${BIN_NAME}" PRIVATE /W4 /permissive-)
else() #elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

  if(NOT APPLE AND NOT EMSCRIPTEN)
    target_compile_options("${BIN_NAME}" PRIVATE -Wall -Wconversion -Wextra -Wpedantic)
  endif()
endif()

############################################
# Download Assets & Extract                #
############################################
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

############################################
# Libraries                                #
############################################
if(NOT EMSCRIPTEN)
  # See 'vcpkg.json' & output of CMake for usage.
  find_package(GLEW REQUIRED)
  find_package(OpenGL REQUIRED) # GLU should be included.
  find_package(SDL2 CONFIG REQUIRED)
  find_package(SDL2_image CONFIG REQUIRED)
  find_package(SDL2_mixer CONFIG REQUIRED)

  target_link_libraries("${BIN_NAME}" PRIVATE
      GLEW::GLEW
      OpenGL::GL
      OpenGL::GLU
      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
      $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
      $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
      $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
  )
endif()

if(RENDERER MATCHES "GLES")
  find_package(glm CONFIG REQUIRED)

  target_link_libraries("${BIN_NAME}" PRIVATE
      glm::glm
  )
endif()

############################################
# Source Code                              #
############################################
target_include_directories("${BIN_NAME}" PRIVATE
    "${TP_DIR}"
    "${SRC_DIR}"
)
# Run `./scripts/glob_src.rb` to show source files to add/remove here.
target_sources("${BIN_NAME}" PRIVATE
#    "${TP_DIR}/Dantares/Dantares.cpp"
    "${TP_DIR}/Dantares/Dantares2.cpp"
    "${TP_DIR}/Dantares/Dantares2GL.cpp"

    "${SRC_DIR}/cybel/audio/audio_player.cpp"
    "${SRC_DIR}/cybel/audio/music.cpp"
    "${SRC_DIR}/cybel/gfx/font_atlas.cpp"
    "${SRC_DIR}/cybel/gfx/image.cpp"
    "${SRC_DIR}/cybel/gfx/renderer.cpp"
    "${SRC_DIR}/cybel/gfx/renderer_gl.cpp"
    "${SRC_DIR}/cybel/gfx/renderer_gles.cpp"
    "${SRC_DIR}/cybel/gfx/sprite.cpp"
    "${SRC_DIR}/cybel/gfx/sprite_atlas.cpp"
    "${SRC_DIR}/cybel/gfx/texture.cpp"
    "${SRC_DIR}/cybel/input/input_man.cpp"
    "${SRC_DIR}/cybel/io/text_reader.cpp"
    "${SRC_DIR}/cybel/io/text_reader_buf.cpp"
    "${SRC_DIR}/cybel/scene/scene_bag.cpp"
    "${SRC_DIR}/cybel/scene/scene_man.cpp"
    "${SRC_DIR}/cybel/str/utf8/rune_iterator.cpp"
    "${SRC_DIR}/cybel/str/utf8/rune_range.cpp"
    "${SRC_DIR}/cybel/str/utf8/rune_util.cpp"
    "${SRC_DIR}/cybel/str/utf8/str_util.cpp"
    "${SRC_DIR}/cybel/stubs/glew_stub.cpp"
    "${SRC_DIR}/cybel/types/color.cpp"
    "${SRC_DIR}/cybel/types/cybel_error.cpp"
    "${SRC_DIR}/cybel/types/duration.cpp"
    "${SRC_DIR}/cybel/types/range.cpp"
    "${SRC_DIR}/cybel/util/rando.cpp"
    "${SRC_DIR}/cybel/util/timer.cpp"
    "${SRC_DIR}/cybel/util/util.cpp"
    "${SRC_DIR}/cybel/vfx/particle.cpp"
    "${SRC_DIR}/cybel/cybel_engine.cpp"

    "${SRC_DIR}/assets/assets.cpp"
    "${SRC_DIR}/assets/font_renderer.cpp"

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
    "${SRC_DIR}/game_context.cpp"
    "${SRC_DIR}/main.cpp"
)

############################################
# Custom Targets                           #
############################################
if(EMSCRIPTEN)
  add_custom_target(run
      COMMAND emrun --verbose --browser firefox "$<TARGET_FILE:${BIN_NAME}>"
      DEPENDS "${BIN_NAME}"
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      USES_TERMINAL
      VERBATIM
  )
elseif()
  add_custom_target(run
      COMMAND "$<TARGET_FILE:${BIN_NAME}>"
      DEPENDS "${BIN_NAME}"
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      USES_TERMINAL # For terminal output during testing.
      VERBATIM
  )
endif()

add_custom_target(check
    COMMAND cppcheck --quiet --enable=all --check-level=exhaustive
            --language=c++ "--std=c++${CMAKE_CXX_STANDARD}"
            "--relative-paths=${CMAKE_SOURCE_DIR}"
            "--checkers-report=${CMAKE_BINARY_DIR}/cppcheck_checkers_report.txt"

            --suppress=missingInclude
            --suppress=missingIncludeSystem
            #"--suppress=noExplicitConstructor:src/cybel/types.h"
            --suppress=shadowFunction
            --suppress=unmatchedSuppression
            --suppress=unusedFunction
            --suppress=unusedPrivateFunction

            "${TP_DIR}"
            "${SRC_DIR}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)
add_custom_target(appimage
    COMMAND "${CMAKE_COMMAND}" -P "${TEMPL_DIR}/appimage.cmake"
    DEPENDS "${BIN_NAME}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM
)
add_custom_target(check_macos_bundle
    COMMAND "${CMAKE_COMMAND}" -P "${TEMPL_DIR}/check_macos_bundle.cmake"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM
)

############################################
# AppImage                                 #
############################################
install(TARGETS "${BIN_NAME}"
    # For AppImage, must use 'bin', cannot use 'games'.
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
install(FILES "${RES_DIR}/${RDNS_NAME}.desktop"
    DESTINATION "share/applications"
    COMPONENT appimage
    EXCLUDE_FROM_ALL
)
install(FILES "${TEMPL_DIR}/${RDNS_NAME}.metainfo.xml"
    DESTINATION "share/metainfo"
    # FIXME: The official AppStream docs recommend `.metainfo.xml` now, but AppImage still only supports
    #        the older `.appdata.xml`. If AppImage possibly fixes this in the future, remove this.
    RENAME "${RDNS_NAME}.appdata.xml"
    COMPONENT appimage
    EXCLUDE_FROM_ALL
)

############################################
# Package (CPack)                          #
############################################
if(APPLE)
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-macos")
  set(CPACK_GENERATOR "Bundle")

  set(PKG_MACOS_BASE_DIR "../../..")
  set(PKG_MACOS_BIN_DIR "../MacOS")

  # NOTE: Can't use EXCLUDE_FROM_ALL with Bundle generator.

  if(PKG_MACOS_BUILD_UNI_DIR)
    # vcpkg doesn't currently support universal macOS binaries,
    #     so making the universal binary manually on GitHub.
    # See: .github/workflows/macos.yml

    # NOTE: This also works, but doesn't appear in verbose output.
#    set(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_SOURCE_DIR}/build_uni/${BIN_NAME}")

    install(PROGRAMS "${CMAKE_SOURCE_DIR}/build_uni/${BIN_NAME}"
        DESTINATION "${PKG_MACOS_BIN_DIR}"
        COMPONENT cpack
    )
  else()
    install(TARGETS "${BIN_NAME}"
        BUNDLE DESTINATION "${PKG_MACOS_BIN_DIR}"
        COMPONENT cpack

        RUNTIME DESTINATION "${PKG_MACOS_BIN_DIR}"
        COMPONENT cpack
    )
  endif()

  install(FILES ${PKG_BASE_FILES}
      DESTINATION "${PKG_MACOS_BASE_DIR}"
      COMPONENT cpack
  )
  install(DIRECTORY ${PKG_RES_DIRS}
      DESTINATION "."
      COMPONENT cpack
  )
  install(FILES ${PKG_RES_FILES}
      DESTINATION "."
      COMPONENT cpack
  )
elseif(WIN32)
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-windows")
  set(CPACK_GENERATOR "ZIP")

  install(TARGETS "${BIN_NAME}"
      RUNTIME DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )

  install(DIRECTORY ${PKG_RES_DIRS}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
  install(FILES ${PKG_BASE_FILES}
                ${PKG_RES_FILES}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
else()
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-linux")
  set(CPACK_GENERATOR "TGZ")

  # Must have a trailing slash! So that the dir is not created in the package.
  install(DIRECTORY "${APPIMG_DIR}/"
      DESTINATION "."
      FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                       GROUP_READ GROUP_EXECUTE
                       WORLD_READ WORLD_EXECUTE
      COMPONENT cpack
      EXCLUDE_FROM_ALL
      FILES_MATCHING PATTERN "*.AppImage"
  )
  # Desktop Entry files should also be executable.
  install(PROGRAMS "${TEMPL_DIR}/${BIN_NAME}.sh"
                   "${RES_DIR}/${RDNS_NAME}.desktop"
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )

  install(DIRECTORY ${PKG_RES_DIRS}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
  install(FILES ${PKG_BASE_FILES}
                ${PKG_RES_FILES}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
endif()

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}${PKG_SUFFIX}")

############################################
# Configure Template Files                 #
############################################
configure_file("${TEMPL_CMAKE_DIR}/appimage.cmake.in" "${TEMPL_DIR}/appimage.cmake" @ONLY)
# NOTE: Must be configured before including CPack.
configure_file("${TEMPL_CMAKE_DIR}/check_macos_bundle.cmake.in" "${TEMPL_DIR}/check_macos_bundle.cmake" @ONLY)

configure_file("${RES_DIR}/.itch.toml" "${TEMPL_DIR}/.itch.toml" @ONLY)
configure_file("${RES_DIR}/${RDNS_NAME}.metainfo.xml" "${TEMPL_DIR}/${RDNS_NAME}.metainfo.xml" @ONLY)
configure_file("${RES_DIR}/Info.plist" "${TEMPL_DIR}/Info.plist" @ONLY)
configure_file("${RES_DIR}/launcher.sh" "${TEMPL_DIR}/${BIN_NAME}.sh" @ONLY)

include(CPack)
