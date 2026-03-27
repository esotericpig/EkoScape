# 'CMakePresets.json' v2 must at least have CMake v3.20.
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
    LANGUAGES C CXX

    HOMEPAGE_URL "https://github.com/esotericpig/EkoScape"
)

#===========================================
# User Config
#===========================================
set(EKO_EXE_DIR_NAME "bin" CACHE STRING "Dir name to output the exe file to.")

# TODO: Rename to APPIMAGE?
set(EKO_LINUXDEPLOY_ARCH "x86_64" CACHE STRING "Arch of `linuxdeploy-<arch>.AppImage` for `appimage` target.")

# TODO: Rename to CPACK?
set(EKO_PKG_SUFFIX "" CACHE STRING "Suffix of package name for CPack.")
option(EKO_PKG_MACOS_USE_BUILD_UNI_DIR "For macOS, use `build_uni` dir for `package` target." OFF)

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
string(TOLOWER "${PROJECT_NAME}" PROJ_ID)
set(RDNS_NAME "io.github.esotericpig.${PROJ_ID}")

set(BIN_NAME "${PROJECT_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${EKO_EXE_DIR_NAME}/$<CONFIG>")

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

set(DOWNLOADS_DIR "${CMAKE_BINARY_DIR}/downloads")
set(RES_DIR "${CMAKE_SOURCE_DIR}/res")

set(ASSETS_NAME "assets")
set(ASSETS_FILENAME "${ASSETS_NAME}.zip")
set(ASSETS_URL "https://github.com/esotericpig/EkoScape/releases/download/v1.99/${ASSETS_FILENAME}")
set(ASSETS_DOWN_FILE "${DOWNLOADS_DIR}/${ASSETS_FILENAME}")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/${ASSETS_NAME}")

set(CONFIG_CMAKE_IN_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(CONFIG_OUT_DIR "${CMAKE_BINARY_DIR}/config")
set(APPIMG_DIR "${CMAKE_BINARY_DIR}/AppImage")

set(PKG_BASE_FILES "${CONFIG_OUT_DIR}/.itch.toml")
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
set(CPACK_BUNDLE_PLIST "${CONFIG_OUT_DIR}/Info.plist")
set(CPACK_BUNDLE_ICON "${ASSETS_DIR}/icons/${PROJECT_NAME}.icns")

set(FETCHCONTENT_QUIET OFF)

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

#===========================================
# Download Assets
#===========================================
if(NOT EXISTS "${ASSETS_DIR}/images/EkoScape.png")
  if(NOT EXISTS "${ASSETS_DOWN_FILE}")
    message(STATUS "Downloading '${ASSETS_URL}' to '${ASSETS_DOWN_FILE}'...")
    file(DOWNLOAD "${ASSETS_URL}" "${ASSETS_DOWN_FILE}"
#        EXPECTED_HASH SHA256=
        TLS_VERIFY ON
        SHOW_PROGRESS
    )
  endif()

  message(STATUS "Extracting '${ASSETS_DOWN_FILE}' to '${ASSETS_DIR}'...")
  file(ARCHIVE_EXTRACT
      INPUT "${ASSETS_DOWN_FILE}"
      DESTINATION "${ASSETS_DIR}"
      PATTERNS "icons" "images" "music" "textures"
#      LIST_ONLY # TEST: Uncomment to list files only, no extraction.
      VERBOSE
  )
endif()

#===========================================
# Project/Build Setup
#===========================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  # Statically link system libs.
  # NOTE: Must set before add_executable()/add_library().
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(EkoScape WIN32)
add_library(EkoScapeBuildConfig INTERFACE)
add_library(EkoScape::BuildConfig ALIAS EkoScapeBuildConfig)

if(EMSCRIPTEN)
  set_target_properties(EkoScape PROPERTIES
      OUTPUT_NAME "${PROJ_ID}"
      SUFFIX ".html"
  )

  # itch.io requires `index.html`, but I want the other files to be named `ekoscape.*`.
  # - Copying (instead of renaming), else linking will always run due to `ekoscape.html' not existing.
  add_custom_command(TARGET EkoScape
      POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy
        "$<TARGET_FILE:EkoScape>"
        "$<TARGET_FILE_DIR:EkoScape>/index.html"
      COMMENT "Copying target file as `index.html`."
      VERBATIM
      USES_TERMINAL
  )
  add_custom_command(TARGET EkoScape
      POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy
        "${ASSETS_DIR}/icons/${PROJECT_NAME}.ico"
        "$<TARGET_FILE_DIR:EkoScape>/favicon.ico"
      COMMENT "Copying favicon to target dir."
      VERBATIM
      USES_TERMINAL
  )

  set(EMS_COMPILE_AND_LINK_OPTS
      # Settings.
      # - https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
      -sDISABLE_EXCEPTION_CATCHING=0
#      -sSTRICT=1
#      -sSUPPORT_LONGJMP=0

      # In Debug, don't minify JS.
      $<$<CONFIG:Debug>:
        -g2
        --minify 0
      >

      # Code optimizations.
      # - https://emscripten.org/docs/optimizing/Optimizing-Code.html
      # - https://emscripten.org/docs/optimizing/Optimizing-WebGL.html
      #
      # NOTE: Optimizations can increase compile/link time. If not desired, comment out.
      $<$<CONFIG:Release>:
        -flto
        -fno-rtti
      >

      # Libraries (ports).
      # - Show available ports:
      #     emcc --show-ports
      # - Show a port's options (e.g., sdl2_image's formats):
      #     emcc --use-port=sdl2_image:help
      --use-port=sdl2
      --use-port=sdl2_image:formats=png
      --use-port=sdl2_mixer
  )

  # - https://emscripten.org/docs/tools_reference/emcc.html
  target_compile_options(EkoScapeBuildConfig INTERFACE
      ${EMS_COMPILE_AND_LINK_OPTS}
  )

  # - https://emscripten.org/docs/tools_reference/emcc.html
  target_link_options(EkoScapeBuildConfig INTERFACE
      ${EMS_COMPILE_AND_LINK_OPTS}

#      --check
      --emrun
      --output_eol linux
      --shell-file "${CONFIG_OUT_DIR}/shell.html"

      # Settings.
      # - https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
      -sALLOW_MEMORY_GROWTH=1
      "-sEXPORTED_FUNCTIONS=_main,ccall,_SDL_ShowCursor"
      -sMINIFY_HTML=0

      $<$<CONFIG:Debug>:
        -sEXCEPTION_DEBUG=1
        -sEXCEPTION_STACK_TRACES=1
        -sGL_ASSERTIONS=1
        -sSAFE_HEAP=1
      >

      # - OpenGL ES 3.0.
      -sMIN_WEBGL_VERSION=2
      -sMAX_WEBGL_VERSION=2
#      -sFULL_ES3=1

      # Using `--preload-file` (instead of `--embed-file`) because it produces a separate `.data` file,
      #   which means if the the game is updated but not the assets, the user won't have to re-download
      #   the assets as they'll be cached by the browser.
      "--preload-file=${ASSETS_DIR}@/${ASSETS_NAME}"
      "--exclude-file=*.icns"
      "--exclude-file=*.ico"
      "--exclude-file=*.md"
      "--exclude-file=*/.gitkeep"
      "--exclude-file=*/images/ekotiles.png"
      "--exclude-file=*/images/keys.png"
      "--exclude-file=*/maps/template_*.txt"
#      --use-preload-plugins # NOTE: Don't use; breaks star textures.
  )
endif()

if(NOT EMSCRIPTEN AND CYBEL_RENDERER STREQUAL "GL")
  target_compile_definitions(EkoScapeBuildConfig INTERFACE DANTARES_RENDERER_GL)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG /utf-8")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

#  target_compile_options(EkoScapeBuildConfig INTERFACE
#      /W4 /permissive-
#  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -finput-charset=UTF-8")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

  target_compile_options(EkoScapeBuildConfig INTERFACE
      $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Emscripten>>:
        -Wall -Wconversion -Wextra -Wpedantic -Wsign-conversion -Wundef
        -fno-common
      >
  )
endif()

target_link_libraries(EkoScape PRIVATE EkoScape::BuildConfig)

#===========================================
# Libraries
#===========================================
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
    "${SRC_DIR}"
)
# Run `./scripts/glob_src.rb` to show source files to add/remove here.
target_sources(EkoScape PRIVATE
    "$<$<BOOL:${WIN32}>:${RES_DIR}/res.rc>"

    # @glob_src=src:${SRC_DIR}/
    "${SRC_DIR}/ekoscape_game.cpp"
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/assets/assets.cpp"
    "${SRC_DIR}/assets/font_renderer.cpp"
    "${SRC_DIR}/map/dantares_map.cpp"
    "${SRC_DIR}/map/facing.cpp"
    "${SRC_DIR}/map/map.cpp"
    "${SRC_DIR}/map/map_grid.cpp"
    "${SRC_DIR}/map/space.cpp"
    "${SRC_DIR}/map/space_type.cpp"
    "${SRC_DIR}/scenes/boring_work_scene.cpp"
    "${SRC_DIR}/scenes/dantares_renderer.cpp"
    "${SRC_DIR}/scenes/game_hud.cpp"
    "${SRC_DIR}/scenes/game_overlay.cpp"
    "${SRC_DIR}/scenes/game_scene.cpp"
    "${SRC_DIR}/scenes/menu_credits_scene.cpp"
    "${SRC_DIR}/scenes/menu_play_scene.cpp"
    "${SRC_DIR}/scenes/menu_scene.cpp"
    "${SRC_DIR}/scenes/scene_action.cpp"
    "${SRC_DIR}/world/robot.cpp"
    "${SRC_DIR}/world/star_sys.cpp"
    # @glob_src
)

#===========================================
# Custom Targets
#===========================================
include(cybel/CustomTargets)

if(EMSCRIPTEN)
  add_custom_target(run
      COMMAND emrun --no-browser "$<TARGET_FILE_DIR:EkoScape>"
      DEPENDS EkoScape
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      USES_TERMINAL
      VERBATIM
  )
else()
  add_custom_target(run
      COMMAND "$<TARGET_FILE:EkoScape>"
      DEPENDS EkoScape
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      USES_TERMINAL # For terminal output during testing.
      VERBATIM
  )
endif()

cybel_add_cppcheck_target(
    ARGS
      --suppress=checkersReport
      --suppress=functionConst
      --suppress=functionStatic
      --suppress=missingInclude
      --suppress=missingIncludeSystem
#      "--suppress=noExplicitConstructor:src/cybel/types/"
      --suppress=shadowFunction
      --suppress=unknownMacro # For Emscripten: EM_ASM(), etc.
      --suppress=unmatchedSuppression
      --suppress=unusedFunction
#      --suppress=unusedPrivateFunction
)
add_custom_target(appimage
    COMMAND "${CMAKE_COMMAND}" -P "${CONFIG_OUT_DIR}/appimage.cmake"
    DEPENDS EkoScape
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM
)
add_custom_target(check_macos_bundle
    COMMAND "${CMAKE_COMMAND}" -P "${CONFIG_OUT_DIR}/check_macos_bundle.cmake"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM
)

#===========================================
# AppImage
#===========================================
install(TARGETS EkoScape
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
install(FILES "${CONFIG_OUT_DIR}/${RDNS_NAME}.metainfo.xml"
    DESTINATION "share/metainfo"
    # FIXME: The official AppStream docs recommend `.metainfo.xml` now, but AppImage still only supports
    #        the older `.appdata.xml`. If AppImage possibly fixes this in the future, remove this.
    RENAME "${RDNS_NAME}.appdata.xml"
    COMPONENT appimage
    EXCLUDE_FROM_ALL
)

#===========================================
# CPack
#===========================================
if(APPLE)
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-macos")
  set(CPACK_GENERATOR "Bundle")

  set(PKG_MACOS_BASE_DIR "../../..")
  set(PKG_MACOS_BIN_DIR "../MacOS")

  # NOTE: Can't use EXCLUDE_FROM_ALL with Bundle generator.

  if(EKO_PKG_MACOS_USE_BUILD_UNI_DIR)
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
    install(TARGETS EkoScape
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

  install(TARGETS EkoScape
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
  install(PROGRAMS "${CONFIG_OUT_DIR}/${BIN_NAME}.sh"
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

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}${EKO_PKG_SUFFIX}")

#===========================================
# Configure Template Files
#===========================================
configure_file("${CONFIG_CMAKE_IN_DIR}/appimage.cmake.in" "${CONFIG_OUT_DIR}/appimage.cmake" @ONLY)
# NOTE: Must be configured before including CPack.
configure_file("${CONFIG_CMAKE_IN_DIR}/check_macos_bundle.cmake.in"
               "${CONFIG_OUT_DIR}/check_macos_bundle.cmake" @ONLY)

configure_file("${RES_DIR}/.itch.toml" "${CONFIG_OUT_DIR}/.itch.toml" @ONLY)
configure_file("${RES_DIR}/Info.plist" "${CONFIG_OUT_DIR}/Info.plist" @ONLY)
configure_file("${RES_DIR}/${RDNS_NAME}.metainfo.xml" "${CONFIG_OUT_DIR}/${RDNS_NAME}.metainfo.xml" @ONLY)
configure_file("${RES_DIR}/launcher.sh" "${CONFIG_OUT_DIR}/${BIN_NAME}.sh" @ONLY)
configure_file("${RES_DIR}/shell.html" "${CONFIG_OUT_DIR}/shell.html" @ONLY)

include(CPack)
