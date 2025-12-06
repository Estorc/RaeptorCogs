#========================================================================#
# RaeptorCogs
# @author Estorc
#========================================================================#

cmake_minimum_required(VERSION 3.14)
project(RaeptorCogs VERSION 1.0 LANGUAGES C CXX)

set(AUTHOR "Estorc")
set(YEAR "2025")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C compiler ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CXX compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C compiler version: ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "CXX compiler version: ${CMAKE_CXX_COMPILER_VERSION}")


#========================================================================#
# CMAKE MODULES
#========================================================================#

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(FileIOUtils)

include(FetchContent)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if(NOT Python3_Interpreter_FOUND)
  message(FATAL_ERROR "Python3 interpreter not found. Please install Python3 to build this project.")
endif()


#========================================================================#
# SOURCES
#========================================================================#

file(GLOB_RECURSE LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file (GLOB_RECURSE HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

#========================================================================#
# Doxygen
#========================================================================#
find_package(Doxygen)

if (DOXYGEN_FOUND)
    message(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")

    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/docs/doxygen/Doxyfile)

    doxygen_add_docs(
        docs-doxygen
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    )

    # configure Doxyfile to point to the right paths
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    set(DOXYGEN_XML "${CMAKE_BINARY_DIR}/docs/doxygen/xml")
    set(DOXYGEN_INDEX "${DOXYGEN_XML}/index.xml")

    add_custom_command(
        OUTPUT ${DOXYGEN_INDEX}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        DEPENDS ${HEADERS}
    )


    add_custom_target(RaeptorCogs_docs-doxygen
    DEPENDS ${DOXYGEN_INDEX}
    COMMENT "Generating Doxygen XML documentation"
    VERBATIM
    )
else()
    message(STATUS "Doxygen not found. Doxygen documentation will not be generated.")
endif()

#========================================================================#
# Sphinx
#========================================================================#
if (DOXYGEN_FOUND)
    find_program(SPHINX_BUILD_EXECUTABLE sphinx-build)
    if(NOT SPHINX_BUILD_EXECUTABLE)
        execute_process(
            COMMAND ${Python3_EXECUTABLE} -m pip show sphinx
            RESULT_VARIABLE SPHINX_PIP_RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(NOT SPHINX_PIP_RESULT EQUAL 0)
            message(STATUS "Python package 'sphinx' not found. Installing...")
            execute_process(
                COMMAND ${Python3_EXECUTABLE} -m pip install --user sphinx
                RESULT_VARIABLE PIP_INSTALL_RESULT
            )
            if(NOT PIP_INSTALL_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to install Python package 'sphinx'. Please install it manually.")
            endif()
        endif()
    else()

        execute_process(
            COMMAND ${Python3_EXECUTABLE} -m pip show furo
            RESULT_VARIABLE FURO_PIP_RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(NOT FURO_PIP_RESULT EQUAL 0)
            message(STATUS "Python package 'furo' not found. Installing...")
            execute_process(
                COMMAND ${Python3_EXECUTABLE} -m pip install --upgrade furo
                RESULT_VARIABLE FURO_INSTALL_RESULT
            )

            if(NOT FURO_INSTALL_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to install Furo theme")
            endif()
        endif()

        set(SPHINX_SOURCE ${CMAKE_BINARY_DIR}/docs/sphinx)
        set(SPHINX_BUILD  ${CMAKE_BINARY_DIR}/docs/html)

        set(SPHINX_IN ${CMAKE_SOURCE_DIR}/docs/sphinx/conf.py.in)
        set(SPHINX_OUT ${CMAKE_BINARY_DIR}/docs/sphinx/conf.py)
        set(DOXYGEN_XML_DIR "${CMAKE_BINARY_DIR}/docs/doxygen/xml")

        configure_file(${SPHINX_IN} ${SPHINX_OUT} @ONLY)
        set_property(DIRECTORY PROPERTY CMAKE_CONFIGURE_DEPENDS "${SPHINX_IN}")
        set_property(SOURCE ${SPHINX_OUT} PROPERTY GENERATED TRUE)

        # Copy rts files to build directory
        copy_folder_if_changed(
            "${CMAKE_SOURCE_DIR}/docs/sphinx/"
            "${CMAKE_BINARY_DIR}/docs/"
            "copy_sphinx"
        )

        copy_folder_if_changed(
            "${CMAKE_SOURCE_DIR}/examples/demo/"
            "${CMAKE_BINARY_DIR}/docs/sphinx/_static/"
            "copy_demo_in_sphinx"
        )

        set(SPHINX_OPTIONS -j auto)

        option(SPHINX_FORCE_REBUILD "Force Sphinx to rebuild all documentation" OFF)
        if (SPHINX_FORCE_REBUILD)
            message(STATUS "Sphinx: Forcing full rebuild of documentation")
            list(APPEND SPHINX_OPTIONS -E)
        endif()

        add_custom_target(RaeptorCogs_docs-sphinx
            COMMAND ${SPHINX_BUILD_EXECUTABLE} ${SPHINX_OPTIONS} -b html ${SPHINX_SOURCE} ${SPHINX_BUILD}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Building Sphinx HTML documentation"
            DEPENDS copy_sphinx copy_demo_in_sphinx
            VERBATIM
        )

        # Make sphinx depend on doxygen XML (so Doxygen runs before Sphinx)
        add_dependencies(RaeptorCogs_docs-sphinx RaeptorCogs_docs-doxygen)

        # Optional: clean and then build
        add_custom_target(RaeptorCogs_docs
            DEPENDS RaeptorCogs_docs-doxygen RaeptorCogs_docs-sphinx
        )
    endif()
endif()

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
  add_compile_options(
      /EHsc
      /W4                    # MSVC warnings
      /clang:-Wall
      /clang:-Wextra
      /clang:-Wpedantic
      /clang:-Wconversion
  )
endif()

if(TARGET_WEBASM)
    set(GL_VERSION "3.0")
    set(GL_PROFILE "core") # still fine for GLES
else()
    set(GL_VERSION "4.6")
    set(GL_PROFILE "core")
endif()

# Helper macro: disable warnings for a target if it exists
function(disable_warnings TARGET_NAME)
    if (TARGET ${TARGET_NAME})
        if (MSVC)
            target_compile_options(${TARGET_NAME} PRIVATE
                /W0
                /clang:-Wno-everything
            )
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${TARGET_NAME} PRIVATE
                -Wno-everything
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${TARGET_NAME} PRIVATE
                -w
            )
        elseif(MSVC)
            target_compile_options(${TARGET_NAME} PRIVATE
                /W0
            )
        endif()
    endif()
endfunction()

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

include(FetchContent)

FetchContent_Declare(
    miniz
    GIT_REPOSITORY https://github.com/richgel999/miniz.git
    GIT_TAG master
)

FetchContent_MakeAvailable(miniz)

if(NOT TARGET_WEBASM)
  # --- Fetch NFD ---
  FetchContent_Declare(
      nfd
      GIT_REPOSITORY  https://github.com/btzy/nativefiledialog-extended.git
  )
  FetchContent_MakeAvailable(nfd)
endif()

find_package(Vulkan REQUIRED)

disable_warnings(glfw)
disable_warnings(glm)
disable_warnings(stb)
disable_warnings(nfd)
disable_warnings(miniz)

#========================================================================#
# BUILD SHADERS
#========================================================================#

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(EMBEDDED_SHADER_HEADER ${CMAKE_CURRENT_BINARY_DIR}/include/RaeptorCogs/EmbedShaders.hpp)
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GLAD_DIR}/include
        ${httplib_SOURCE_DIR}
        ${nfd_SOURCE_DIR}/src/include
        ${miniz_SOURCE_DIR}/source
)

#========================================================================#
# LINK SHADERS
#========================================================================#

add_custom_target(embed_shaders DEPENDS ${EMBEDDED_SHADER_HEADER})
add_dependencies(RaeptorCogs embed_shaders)

#========================================================================#
# IMGUI
#========================================================================#

option(RAEPTORCOGS_BUILD_IMGUI "Build with ImGui support" ON)

if(RAEPTORCOGS_BUILD_IMGUI)

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
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    )

    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${GLAD_DIR}/include
        ${glfw_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIR}
    )

    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

    target_include_directories(RaeptorCogs PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )

    target_compile_options(imgui PRIVATE
        -w            # disable all warnings
    )

    target_link_libraries(RaeptorCogs PUBLIC imgui)
    target_compile_definitions(RaeptorCogs PUBLIC USE_IMGUI)
endif()

#========================================================================#
# LINKING
#========================================================================#
target_link_libraries(RaeptorCogs 
    PUBLIC
        Vulkan::Vulkan
        glm
        glad
        httplib
        ${CMAKE_DL_LIBS}
    PRIVATE
        miniz
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

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
)

FetchContent_MakeAvailable(json)

function(add_example NAME)
    set(options)
    set(oneValueArgs FOLDER)
    set(multiValueArgs SOURCES ASSETS)
    cmake_parse_arguments(EXAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # If user did not provide explicit SOURCES, auto-glob from folder
    if(NOT EXAMPLE_SOURCES)
        file(GLOB_RECURSE EXAMPLE_CPP_SOURCES
            "${EXAMPLE_FOLDER}/src/*.cpp"
            "${EXAMPLE_FOLDER}/src/*.c"
            "${EXAMPLE_FOLDER}/src/*.cc"
            "${EXAMPLE_FOLDER}/src/*.cxx"
        )
    else()
        set(EXAMPLE_CPP_SOURCES ${EXAMPLE_SOURCES})
    endif()

    add_executable(${NAME} ${EXAMPLE_CPP_SOURCES})

    if (WIN32)
        # Windows icon via resource file
        target_sources(${NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/examples/resources.rc
        )
    else()
        # Linux desktop entry + icon
        install(FILES ${CMAKE_SOURCE_DIR}/examples/raeptor-cogs-icon-16.png DESTINATION share/icons/hicolor/16x16/apps RENAME ${NAME}.png)
        install(FILES ${CMAKE_SOURCE_DIR}/examples/raeptor-cogs-icon-32.png DESTINATION share/icons/hicolor/32x32/apps RENAME ${NAME}.png)
        install(FILES ${CMAKE_SOURCE_DIR}/examples/raeptor-cogs-icon-48.png DESTINATION share/icons/hicolor/48x48/apps RENAME ${NAME}.png)
        install(FILES ${CMAKE_SOURCE_DIR}/examples/raeptor-cogs-icon-128.png DESTINATION share/icons/hicolor/128x128/apps RENAME ${NAME}.png)

        # Create app.desktop file in build dir
        configure_file(${CMAKE_SOURCE_DIR}/examples/app.desktop.in
            ${CMAKE_BINARY_DIR}/${NAME}.desktop
            @ONLY
        )
        
        # Install app.desktop file
        install(FILES ${CMAKE_BINARY_DIR}/${NAME}.desktop
            DESTINATION share/applications)
    endif()

    target_link_libraries(${NAME} PRIVATE
        RaeptorCogs
        nlohmann_json::nlohmann_json
    )

    # Include folder
    if (EXAMPLE_FOLDER)
        target_include_directories(${NAME} PRIVATE ${EXAMPLE_FOLDER}/include)
    endif()

    # WASM special handling
    if (TARGET_WEBASM)
        set_target_properties(${NAME} PROPERTIES
            SUFFIX ".html"
            OUTPUT_NAME "${NAME}"
        )
    endif()

    # Copy assets
    foreach(asset ${EXAMPLE_ASSETS})
        # Skip empty
        if(NOT asset)
            continue()
        endif()

        # Resolve absolute path
        get_filename_component(asset_abs "${asset}" ABSOLUTE)
        if(NOT EXISTS "${asset_abs}")
            message(WARNING "add_example(${NAME}): asset directory does not exist: ${asset_abs}")
            continue()
        endif()

        # basename to create nice subfolder in binary dir
        get_filename_component(asset_base "${asset_abs}" NAME)

        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${asset_abs}"
                "$<TARGET_FILE_DIR:${NAME}>/${asset_base}"
            COMMENT "Copying ${asset_base} -> $<TARGET_FILE_DIR:${NAME}>/${asset_base}"
        )
    endforeach()
endfunction()

add_example(RaeptorCogsTestPlayground
    FOLDER ${CMAKE_SOURCE_DIR}/examples/test-playground
    ASSETS
        ${CMAKE_SOURCE_DIR}/examples/test-playground/assets
)

add_example(RaeptorCogsDemo
    FOLDER ${CMAKE_SOURCE_DIR}/examples/demo
    ASSETS
        ${CMAKE_SOURCE_DIR}/examples/demo/assets
)

#========================================================================#
# TESTING
#========================================================================#

enable_testing()
add_subdirectory(tests)

#========================================================================#