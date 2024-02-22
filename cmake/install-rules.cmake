install(
    TARGETS OpenGLLayeredWindow
    RUNTIME COMPONENT OpenGLLayeredWindow_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
