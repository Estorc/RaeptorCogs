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


# --- Add GLAD static library (using pre-generated sources) ---
add_library(glad STATIC
    glad/src/gl.c
)
target_include_directories(glad PUBLIC glad/include)

find_package(OpenGL REQUIRED)

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
    glad/include
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
    third_party
    glad/include
    shaders
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