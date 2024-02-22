cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    OpenGLLayeredWindow
    VERSION 0.1.0
    DESCRIPTION "Demonstrate how to use OpenGL to draw a layered window with alpha blending."
    #HOMEPAGE_URL "https://example.com/"
    #LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(glad
    3rdparty/glad-3.3-core/src/glad.c
    3rdparty/glad-3.3-core/src/glad_wgl.c
)

target_include_directories(
    glad ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rdparty/glad-3.3-core/include>"
)

# ---- Declare executable ----

add_executable(OpenGLLayeredWindow WIN32
	source/main.cpp
	source/View.cpp
	source/io/ReadFile.cpp
	source/io/Wavefront.cpp
	source/pix/Pixmap.cpp
	source/wex/Bitmap.cpp
	source/wex/DIBSection.cpp
	source/wex/Resource.cpp
	source/wex/Window.cpp
	source/wex/WindowClass.cpp
	source/wgl/OpenGLContext.cpp
)
add_executable(OpenGLLayeredWindow::exe ALIAS OpenGLLayeredWindow)
target_sources(OpenGLLayeredWindow PRIVATE res/Resource.rc)

set_property(TARGET OpenGLLayeredWindow PROPERTY OUTPUT_NAME OpenGLLayeredWindow)

target_compile_features(OpenGLLayeredWindow PRIVATE cxx_std_20)

target_precompile_headers(OpenGLLayeredWindow PRIVATE source/stdafx.hpp)

#add_compile_definitions(OpenGLLayeredWindow _UNICODE UNICODE)

target_link_libraries(OpenGLLayeredWindow 
	PRIVATE 
	glad
	OpenGL32
	ShCore
)

target_include_directories(
    OpenGLLayeredWindow ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rdparty>"
    #"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/wex>"
)

add_compile_definitions(_UNICODE UNICODE)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT OpenGLLayeredWindow)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT OpenGLLayeredWindow_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of OpenGLLayeredWindow"
  )
endif()

include(cmake/dev-mode.cmake)
