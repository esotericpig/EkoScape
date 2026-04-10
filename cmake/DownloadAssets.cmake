###
# @author Bradley Whited
###

if(NOT EXISTS "${ASSETS_DIR}/images/EkoScape.png")
  set(assets_file_name "${ASSETS_NAME}.zip")
  set(assets_url "https://github.com/esotericpig/EkoScape/releases/download/v1.99/${assets_file_name}")
  set(assets_down_file "${DOWNLOADS_DIR}/${assets_file_name}")

  if(NOT EXISTS "${assets_down_file}")
    message(STATUS "Downloading '${assets_url}' to '${assets_down_file}'...")
    file(DOWNLOAD "${assets_url}" "${assets_down_file}"
#        EXPECTED_HASH SHA256=
        TLS_VERIFY ON
        SHOW_PROGRESS
    )
  endif()

  message(STATUS "Extracting '${assets_down_file}' to '${ASSETS_DIR}'...")
  file(ARCHIVE_EXTRACT
      INPUT "${assets_down_file}"
      DESTINATION "${ASSETS_DIR}"
      PATTERNS "icons" "images" "music" "textures"
#      LIST_ONLY # TEST: Uncomment to list files only, no extraction.
      VERBOSE
  )
endif()
