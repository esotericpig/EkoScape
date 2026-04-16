###
# @author Bradley Whited
###

if(APPLE)
  set(EKO_ITCHIO_PLAY_PATH "${EKO_EXE_NAME}.app")
  set(EKO_ITCHIO_RES_PATH "./${EKO_EXE_NAME}.app/Contents/Resources")
elseif(WIN32)
  set(EKO_ITCHIO_PLAY_PATH "${EKO_EXE_NAME}.exe")
  set(EKO_ITCHIO_RES_PATH ".")
else() # Linux.
  set(EKO_ITCHIO_PLAY_PATH "${EKO_EXE_NAME}.sh")
  set(EKO_ITCHIO_RES_PATH ".")
endif()

#===========================================
# Configure Template Files
#===========================================
configure_file(
    "${EKO_RES_DIR}/.itch.toml.in"
    "${EKO_GEN_DIR}/.itch.toml"
    @ONLY
)
