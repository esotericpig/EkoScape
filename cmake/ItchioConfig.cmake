###
# @author Bradley Whited
###

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
# Configure Template Files
#===========================================
configure_file("${RES_DIR}/.itch.toml.in" "${CONFIG_OUT_DIR}/.itch.toml" @ONLY)
