###
# @author Bradley Whited
###

set_target_properties(EkoScape PROPERTIES
#    OUTPUT_NAME "ekoscape"
    SUFFIX ".html"
)

set(web_compile_and_link_opts
    # Settings.
    # - https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
    -sDISABLE_EXCEPTION_CATCHING=0
#    -sSTRICT=1
#    -sSUPPORT_LONGJMP=0

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
    ${web_compile_and_link_opts}
)

# - https://emscripten.org/docs/tools_reference/emcc.html
target_link_options(EkoScapeBuildConfig INTERFACE
    ${web_compile_and_link_opts}

#    --check
    --emrun
    --output_eol linux
    --shell-file "${EKO_GEN_BUNS_DIR}/web/shell.html"

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
#    -sFULL_ES3=1

    # Using `--preload-file` (instead of `--embed-file`) because it produces a separate `.data` file,
    # which means if the the game is updated, but not the assets, the user won't have to re-download
    # the assets as they'll already be cached by the browser.
    "--preload-file=${EKO_ASSETS_DIR}@/${EKO_ASSETS_NAME}"
    "--exclude-file=*.icns"
    "--exclude-file=*.ico"
    "--exclude-file=*.md"
    "--exclude-file=*/.gitkeep"
    "--exclude-file=*/images/ekotiles.png"
    "--exclude-file=*/images/keys.png"
    "--exclude-file=*/maps/template_*.txt"
#    --use-preload-plugins # NOTE: Don't use; breaks star textures.
)

#===========================================
# Custom Commands
#===========================================
# Itch.io requires `index.html`, but I want the other files to be named `EkoScape.*`.
# Copying (instead of renaming), else linking will always run due to `EkoScape.html` not existing.
add_custom_command(TARGET EkoScape
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
      "$<TARGET_FILE:EkoScape>"
      "$<TARGET_FILE_DIR:EkoScape>/index.html"
    COMMENT "Copying target file as 'index.html'."
    VERBATIM
    USES_TERMINAL
)
add_custom_command(TARGET EkoScape
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
      "${EKO_ASSETS_DIR}/icons/${PROJECT_NAME}.ico"
      "$<TARGET_FILE_DIR:EkoScape>/favicon.ico"
    COMMENT "Copying favicon to target dir."
    VERBATIM
    USES_TERMINAL
)

#===========================================
# Configure Template Files
#===========================================
configure_file(
    "${EKO_BUNS_DIR}/web/shell.html.in"
    "${EKO_GEN_BUNS_DIR}/web/shell.html"
    @ONLY
    NEWLINE_STYLE LF
)
