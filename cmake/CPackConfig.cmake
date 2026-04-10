###
# @author Bradley Whited
###

# TODO: Just loop over and check if a dir or a file? Or something else?
set(pkg_base_files
    "${CONFIG_OUT_DIR}/.itch.toml"
)
set(pkg_res_dirs
    "${ASSETS_DIR}"
)
set(pkg_res_files
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

if(APPLE)
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-macos")
  set(CPACK_GENERATOR "Bundle")

  set(PKG_MACOS_BASE_DIR "../../..")
  set(PKG_MACOS_BIN_DIR "../MacOS")

  # NOTE: Can't use EXCLUDE_FROM_ALL with Bundle generator.

  if(EKO_PKG_MACOS_USE_BUILD_UNI_DIR)
    # vcpkg doesn't currently support universal macOS binaries,
    # so making the universal binary manually on GitHub.
    # - See: .github/workflows/macos.yml

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

  install(FILES ${pkg_base_files}
      DESTINATION "${PKG_MACOS_BASE_DIR}"
      COMPONENT cpack
  )
  install(DIRECTORY ${pkg_res_dirs}
      DESTINATION "."
      COMPONENT cpack
  )
  install(FILES ${pkg_res_files}
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

  install(DIRECTORY ${pkg_res_dirs}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
  install(FILES ${pkg_base_files}
                ${pkg_res_files}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
else() # Linux.
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-linux")
  set(CPACK_GENERATOR "TGZ")

  # NOTE: Must have a trailing slash! So that the dir is not created in the package.
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

  install(DIRECTORY ${pkg_res_dirs}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
  install(FILES ${pkg_base_files}
                ${pkg_res_files}
      DESTINATION "."
      COMPONENT cpack
      EXCLUDE_FROM_ALL
  )
endif()

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}${EKO_PKG_SUFFIX}")

#===========================================
# Custom Targets
#===========================================
add_custom_target(check_macos_bundle
    COMMAND "${CMAKE_COMMAND}" -P "${CONFIG_OUT_DIR}/check_macos_bundle.cmake"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM
)

#===========================================
# Configure Template Files
#===========================================
configure_file("${RES_DIR}/Info.plist" "${CONFIG_OUT_DIR}/Info.plist"
    @ONLY
    NEWLINE_STYLE LF
)
# NOTE: Must be configured before including CPack.
configure_file(
    "${CONFIG_CMAKE_IN_DIR}/check_macos_bundle.cmake.in"
    "${CONFIG_OUT_DIR}/check_macos_bundle.cmake"
    @ONLY
)

include(CPack)
