#========================================================================#
# RaeptorLab
# @author Estorc
#========================================================================#

cmake_minimum_required(VERSION 3.14)
project(RaeptorLab VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

set(GL_VERSION 4.6)
set(GL_PROFILE core)

# Run glad generator during build (custom command)
add_custom_command(
  OUTPUT ${GLAD_DIR}/src/glad.c ${GLAD_DIR}/include/glad/glad.h
  COMMAND ${Python3_EXECUTABLE} -m glad --generator=c --spec=gl --out-path=${GLAD_DIR} --api=gl=${GL_VERSION} --profile=${GL_PROFILE} --extensions=""
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
FetchContent_Declare(
    glfw
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG         latest
)
FetchContent_MakeAvailable(glfw)

# --- Fetch ImGUI ---
FetchContent_Declare(
    imgui
    GIT_REPOSITORY  https://github.com/ocornut/imgui.git
    GIT_TAG         v1.89.8  # or latest stable tag
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    glm
    GIT_REPOSITORY  https://github.com/g-truc/glm.git        
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    nfd
    GIT_REPOSITORY  https://github.com/btzy/nativefiledialog-extended.git
)
FetchContent_MakeAvailable(nfd)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

#========================================================================#
# BUILD SHADERS
#========================================================================#

set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(EMBEDDED_SHADER_HEADER ${CMAKE_BINARY_DIR}/shaders_embed.h)
file(GLOB_RECURSE SHADER_SOURCES "${SHADER_DIR}/*.glsl" "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.fs" "${SHADER_DIR}/*.vs")

add_custom_command(
    OUTPUT ${EMBEDDED_SHADER_HEADER}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/process_shaders.py ${EMBEDDED_SHADER_HEADER} ${SHADER_SOURCES}
    DEPENDS ${SHADER_SOURCES} ${CMAKE_SOURCE_DIR}/process_shaders.py
    COMMENT "Embedding shaders into ${EMBEDDED_SHADER_HEADER}"
    VERBATIM
)

#========================================================================#
# SOURCES
#========================================================================#


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
    ${nfd_SOURCE_DIR}/src/include
)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)


add_executable(RaeptorLab src/main.cpp)

# --- Recursively find all source files in src/ directory ---

file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

target_sources(RaeptorLab PRIVATE ${SOURCES})

# ---- Include dirs ----
target_include_directories(RaeptorLab PRIVATE
    include
    ${GLAD_DIR}/include
    shaders
    ${stb_SOURCE_DIR}
)

#========================================================================#
# LINK SHADERS
#========================================================================#

add_custom_target(embed_shaders DEPENDS ${EMBEDDED_SHADER_HEADER})
add_dependencies(RaeptorLab embed_shaders)
target_include_directories(RaeptorLab PRIVATE ${CMAKE_BINARY_DIR})

#========================================================================#
# LINKING
#========================================================================#

target_link_libraries(RaeptorLab PRIVATE
    glad
    glfw
    imgui
    glm
    nfd
    ${CMAKE_DL_LIBS}
)