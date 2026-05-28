###
# @author Bradley Whited
###

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
#      --suppress=useStlAlgorithm
#      --suppress=variableScope
)
