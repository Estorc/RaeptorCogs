#========================================================================#
# RaeptorCogs
# @author Estorc
#========================================================================#

cmake_minimum_required(VERSION 3.14)
project(RaeptorCogs VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#========================================================================#
# WebAssembly
#========================================================================#

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    set(TARGET_WEBASM TRUE)

    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)

    set(GLFW_USE_PLATFORM_NULL OFF CACHE BOOL " " FORCE)  # Disable null backend
    set(GLFW_USE_PLATFORM_EGL ON CACHE BOOL " " FORCE)    # Use EGL (for GLES)
    set(GLFW_USE_PLATFORM_X11 OFF CACHE BOOL " " FORCE)
    set(GLFW_USE_PLATFORM_WAYLAND OFF CACHE BOOL " " FORCE)
    set(GLFW_USE_PLATFORM_WIN32 OFF CACHE BOOL " " FORCE)
    set(GLFW_USE_PLATFORM_COCOA OFF CACHE BOOL " " FORCE)

    # Compile flags (if needed)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # Link flags
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGL2=1")

    add_definitions(-DCLOCK_MONOTONIC=1)
    add_definitions(-DCLOCK_REALTIME=1)
else()
    set(TARGET_WEBASM FALSE)
endif()

#========================================================================#
# CMAKE CONFIGURATION
#========================================================================#

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if (MSVC)
  set(ENV{VSLANG} "1033")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_compile_options(/wd4005)  # Désactive "macro redefinition"
  add_compile_options(/wd4244 /wd4267) # Désactive "conversion de type possible"
  add_compile_options(/wd4100)
endif()

if(TARGET_WEBASM)
    set(GL_VERSION "3.0")
    set(GL_PROFILE "core") # still fine for GLES
else()
    set(GL_VERSION "4.6")
    set(GL_PROFILE "core")
endif()

#========================================================================#
# CMAKE MODULES
#========================================================================#

include(FetchContent)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if(NOT Python3_Interpreter_FOUND)
  message(FATAL_ERROR "Python3 interpreter not found. Please install Python3 to build this project.")
endif()

#========================================================================#
# GLAD
#========================================================================#

execute_process(
  COMMAND ${Python3_EXECUTABLE} -m pip show glad
  RESULT_VARIABLE glad_pip_result
  OUTPUT_QUIET
  ERROR_QUIET
)

if(NOT glad_pip_result EQUAL 0)
  message(STATUS "Python package 'glad' not found. Installing...")
  execute_process(
    COMMAND ${Python3_EXECUTABLE} -m pip install --user glad
    RESULT_VARIABLE pip_install_result
  )
  if(NOT pip_install_result EQUAL 0)
    message(FATAL_ERROR "Failed to install Python package 'glad'. Please install it manually.")
  endif()
endif()

# Define where to generate glad files
set(GLAD_DIR ${CMAKE_BINARY_DIR}/glad)
file(MAKE_DIRECTORY ${GLAD_DIR})


if(TARGET_WEBASM)
  set(GLAD_COMMAND
    ${Python3_EXECUTABLE} -m glad
    --generator=c
    --spec=gl
    --out-path=${GLAD_DIR}
    --api=gles2=${GL_VERSION}
    --profile=${GL_PROFILE}
    --extensions=""
  )
else()
  set(GLAD_COMMAND
    ${Python3_EXECUTABLE} -m glad
    --generator=c
    --spec=gl
    --out-path=${GLAD_DIR}
    --api=gl=${GL_VERSION}
    --profile=${GL_PROFILE}
    --extensions=""
  )
endif()

# Run glad generator during build (custom command)
add_custom_command(
  OUTPUT ${GLAD_DIR}/src/glad.c ${GLAD_DIR}/include/glad/glad.h
  COMMAND ${GLAD_COMMAND}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating GLAD files"
  VERBATIM
)

# Create glad library from generated files
add_library(glad STATIC
  ${GLAD_DIR}/src/glad.c
)

target_include_directories(glad PUBLIC
    ${GLAD_DIR}/include
)

#========================================================================#
# IMPORTS
#========================================================================#

# --- Fetch GLFW ---
if(NOT TARGET_WEBASM)
  FetchContent_Declare(
      glfw
      GIT_REPOSITORY  https://github.com/glfw/glfw.git
      GIT_TAG         latest
  )
  FetchContent_MakeAvailable(glfw)
endif()


FetchContent_Declare(
    glm
    GIT_REPOSITORY  https://github.com/g-truc/glm.git        
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG        master  # or a specific commit/tag for stability
)
FetchContent_MakeAvailable(httplib)

if(NOT TARGET_WEBASM)
  # --- Fetch NFD ---
  FetchContent_Declare(
      nfd
      GIT_REPOSITORY  https://github.com/btzy/nativefiledialog-extended.git
  )
  FetchContent_MakeAvailable(nfd)
endif()

#========================================================================#
# BUILD SHADERS
#========================================================================#

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(EMBEDDED_SHADER_HEADER ${CMAKE_BINARY_DIR}/shaders_embed.h)
file(GLOB_RECURSE SHADER_SOURCES "${SHADER_DIR}/*.glsl" "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.fs" "${SHADER_DIR}/*.vs")

add_custom_command(
    OUTPUT ${EMBEDDED_SHADER_HEADER}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/process_shaders.py ${EMBEDDED_SHADER_HEADER} ${SHADER_SOURCES}
    DEPENDS ${SHADER_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/process_shaders.py
    COMMENT "Embedding shaders into ${EMBEDDED_SHADER_HEADER}"
    VERBATIM
)

#========================================================================#
# SOURCES
#========================================================================#


# --- Recursively find all source files in src/ directory ---

file(GLOB_RECURSE LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(RaeptorCogs STATIC ${LIB_SOURCES})

if (TARGET_WEBASM)
  set_target_properties(RaeptorCogs PROPERTIES
      LINK_FLAGS "-s -s USE_GLFW=3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1"
  )
endif()

# OpenSSL if needed
find_package(OpenSSL)
if(OpenSSL_FOUND)
    target_compile_definitions(RaeptorCogs PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
    target_link_libraries(RaeptorCogs PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

# ---- Include dirs ----
target_include_directories(RaeptorCogs 
    PUBLIC
        ${stb_SOURCE_DIR}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GLAD_DIR}/include
        ${httplib_SOURCE_DIR}
        ${nfd_SOURCE_DIR}/src/include
)

#========================================================================#
# LINK SHADERS
#========================================================================#

add_custom_target(embed_shaders DEPENDS ${EMBEDDED_SHADER_HEADER})
add_dependencies(RaeptorCogs embed_shaders)
target_include_directories(RaeptorCogs PRIVATE ${CMAKE_BINARY_DIR})

#========================================================================#
# IMGUI
#========================================================================#

option(RAEPTORLAB_BUILD_IMGUI "Build with ImGui support" ON)

if(RAEPTORLAB_BUILD_IMGUI)

    # --- Fetch ImGUI ---
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY  https://github.com/ocornut/imgui.git
        GIT_TAG         v1.89.8  # or latest stable tag
    )
    FetchContent_MakeAvailable(imgui)

    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )

    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${GLAD_DIR}/include
        ${glfw_SOURCE_DIR}/include
    )

    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

    target_include_directories(RaeptorCogs PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )

    target_link_libraries(RaeptorCogs PUBLIC imgui)
    target_compile_definitions(RaeptorCogs PUBLIC USE_IMGUI)
endif()

#========================================================================#
# LINKING
#========================================================================#

target_link_libraries(RaeptorCogs 
    PUBLIC
        glm
        glad
        httplib
        ${CMAKE_DL_LIBS}
)

if(NOT TARGET_WEBASM)
  target_link_libraries(RaeptorCogs 
      PUBLIC
          glfw
          nfd
  )
endif()

#========================================================================#
# EXAMPLES
#========================================================================#

option(RAEPTORLAB_BUILD_EXAMPLES "Build example apps" OFF)

if(RAEPTORLAB_BUILD_EXAMPLES)

    FetchContent_Declare(
      json
      GIT_REPOSITORY https://github.com/nlohmann/json.git
    )

    FetchContent_MakeAvailable(json)


    add_executable(RaeptorLabDemo examples/demo/src/main.cpp)

    file(GLOB_RECURSE SRC_FILES
        "${CMAKE_SOURCE_DIR}/examples/demo/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/examples/demo/src/*.c"
        "${CMAKE_SOURCE_DIR}/examples/demo/include/*.hpp"
    )

    target_sources(RaeptorLabDemo PRIVATE ${SRC_FILES})
    target_include_directories(RaeptorLabDemo PRIVATE ${CMAKE_SOURCE_DIR}/examples/demo/include)

    if (TARGET_WEBASM)
      set_target_properties(RaeptorLabDemo PROPERTIES
          SUFFIX ".html"
          OUTPUT_NAME "RaeptorLabDemo"
          LINK_FLAGS "-s USE_GLFW=3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -s EXPORTED_FUNCTIONS=[_malloc,_free,_main,_file_callback_bridge] -s EXPORTED_RUNTIME_METHODS=[HEAPU8,ccall] --shell-file ${CMAKE_SOURCE_DIR}/src/emscripten/shell.html"
      )
      target_link_options(RaeptorLabDemo PRIVATE
          "--preload-file=${CMAKE_SOURCE_DIR}/examples/demo/assets@/assets"
      )
    endif()

    target_link_libraries(RaeptorLabDemo PRIVATE
        RaeptorCogs
        nlohmann_json::nlohmann_json
    )

    # Copy assets (textures, shaders, etc.)
    add_custom_command(TARGET RaeptorLabDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/examples/demo/assets
            $<TARGET_FILE_DIR:RaeptorLabDemo>/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/examples/demo/protected
            $<TARGET_FILE_DIR:RaeptorLabDemo>/protected
    )

endif()

#========================================================================#