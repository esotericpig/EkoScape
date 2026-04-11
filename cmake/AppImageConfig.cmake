###
# @author Bradley Whited
###

# TODO: Use GNU install dirs?
# TODO: Add custom target for running the AppImage?

install(TARGETS EkoScape
    # NOTE: For AppImage, must use `bin`, cannot use `games`.
    RUNTIME DESTINATION bin
    COMPONENT appimage
    EXCLUDE_FROM_ALL

    # For static libs.
    ARCHIVE DESTINATION lib
    COMPONENT appimage
    EXCLUDE_FROM_ALL

    # For shared libs.
    LIBRARY DESTINATION lib
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
# Custom Targets
#===========================================
add_custom_target(appimage
    COMMAND "${CMAKE_COMMAND}" -P "${CONFIG_OUT_DIR}/BuildAppImage.cmake"
    DEPENDS EkoScape
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    USES_TERMINAL
    VERBATIM
)

#===========================================
# Configure Template Files
#===========================================
configure_file("${RES_DIR}/${RDNS_NAME}.metainfo.xml.in" "${CONFIG_OUT_DIR}/${RDNS_NAME}.metainfo.xml"
    @ONLY
    NEWLINE_STYLE LF
)
configure_file("${CONFIG_CMAKE_IN_DIR}/BuildAppImage.cmake.in" "${CONFIG_OUT_DIR}/BuildAppImage.cmake" @ONLY)
configure_file("${RES_DIR}/${BIN_NAME}.sh.in" "${CONFIG_OUT_DIR}/${BIN_NAME}.sh"
    @ONLY
    NEWLINE_STYLE LF
)
