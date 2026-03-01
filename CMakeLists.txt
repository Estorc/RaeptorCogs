#========================================================================#
# RaeptorCogs
# @author Estorc
#========================================================================#

cmake_minimum_required(VERSION 3.14)
project(RaeptorCogs VERSION 1.0 LANGUAGES C CXX)

set(AUTHOR "Estorc")
set(YEAR "2025")
set(CMAKE_CXX_STANDARD 17)
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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FileIOUtils)

include(FetchContent)

# Support venv
# ---- Python from venv (project-local) ----
set(Python3_EXECUTABLE "${CMAKE_SOURCE_DIR}/venv/bin/python3.14" CACHE FILEPATH "Python from venv" FORCE)
# ------------------------------------------

find_package(Python3 COMPONENTS Interpreter)

if(NOT Python3_FOUND)
    message(FATAL_ERROR "Python3 not found. Please install Python3 and ensure it is in your PATH.")
else()
    message(STATUS "Python3 found: ${Python3_EXECUTABLE}")
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

    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/docs/doxygen/Doxyfile)

    doxygen_add_docs(
        docs-doxygen
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
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
            message(STATUS "Python package 'sphinx' not found.")
            message(FATAL_ERROR "Please install it manually.")
        endif()
    else()

        execute_process(
            COMMAND ${Python3_EXECUTABLE} -m pip show furo
            RESULT_VARIABLE FURO_PIP_RESULT
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(NOT FURO_PIP_RESULT EQUAL 0)
            message(STATUS "Python package 'furo' not found.")
            message(FATAL_ERROR "Please install it manually.")
        endif()

        set(SPHINX_SOURCE ${CMAKE_BINARY_DIR}/docs/sphinx)
        set(SPHINX_BUILD  ${CMAKE_BINARY_DIR}/docs/html)

        set(SPHINX_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/sphinx/conf.py.in)
        set(SPHINX_OUT ${CMAKE_BINARY_DIR}/docs/sphinx/conf.py)
        set(DOXYGEN_XML_DIR "${CMAKE_BINARY_DIR}/docs/doxygen/xml")

        configure_file(${SPHINX_IN} ${SPHINX_OUT} @ONLY)
        set_property(DIRECTORY PROPERTY CMAKE_CONFIGURE_DEPENDS "${SPHINX_IN}")
        set_property(SOURCE ${SPHINX_OUT} PROPERTY GENERATED TRUE)

        # Copy rts files to build directory
        copy_folder_if_changed(
            "${CMAKE_CURRENT_SOURCE_DIR}/docs/sphinx/"
            "${CMAKE_BINARY_DIR}/docs/"
            "copy_sphinx"
        )

        copy_folder_if_changed(
            "${CMAKE_CURRENT_SOURCE_DIR}/examples/demo/"
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
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
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
    message(STATUS "Python package 'glad' not found.")
    message(FATAL_ERROR "Please install it manually.")
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
      GIT_TAG         3.4
  )
  FetchContent_MakeAvailable(glfw)
endif()


FetchContent_Declare(
    glm
    GIT_REPOSITORY  https://github.com/g-truc/glm.git
    GIT_TAG         1.0.3        
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
  GIT_TAG        v0.30.1  # or a specific commit/tag for stability
)
FetchContent_MakeAvailable(httplib)

include(FetchContent)

FetchContent_Declare(
    miniz
    GIT_REPOSITORY https://github.com/richgel999/miniz.git
    GIT_TAG 3.1.0
)

FetchContent_MakeAvailable(miniz)

FetchContent_Declare(
    rgsl
    GIT_REPOSITORY https://github.com/estorc/rgsl.git
    GIT_TAG development
)

FetchContent_MakeAvailable(rgsl)

if(NOT TARGET_WEBASM)
  # --- Fetch NFD ---
  FetchContent_Declare(
      nfd
      GIT_REPOSITORY  https://github.com/btzy/nativefiledialog-extended.git
      GIT_TAG         v1.3.0
  )
  FetchContent_MakeAvailable(nfd)
endif()

find_package(Vulkan)
find_package(OpenSSL)
find_package(TBB QUIET)
find_package(PkgConfig)
if (PkgConfig_FOUND)
    message(STATUS "PkgConfig found: ${PKG_CONFIG_EXECUTABLE}")
    pkg_check_modules(WAYLAND REQUIRED wayland-client)
else()
    message(FATAL_ERROR "PkgConfig not found. Please install PkgConfig.")
endif()

disable_warnings(glfw)
disable_warnings(glm)
disable_warnings(stb)
disable_warnings(nfd)
disable_warnings(miniz)
disable_warnings(rgsl)

#========================================================================#
# BUILD SHADERS
#========================================================================#

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders)
file(GLOB_RECURSE SHADER_SOURCES
    "${SHADER_DIR}/*.vert"
    "${SHADER_DIR}/*.frag"
    "${SHADER_DIR}/*.fs"
    "${SHADER_DIR}/*.vs"
)

set(NORMAL_SHADER_SOURCES ${SHADER_SOURCES})
set(SPIRV_SHADER_SOURCES ${SHADER_SOURCES})

list(FILTER NORMAL_SHADER_SOURCES EXCLUDE REGEX "${SHADER_DIR}/.*/common/.*") # Exclude common shaders
list(FILTER NORMAL_SHADER_SOURCES EXCLUDE REGEX "${SHADER_DIR}/.*spir-v/.*") # Exclude spir-v shaders from normal shaders

list(FILTER SPIRV_SHADER_SOURCES INCLUDE REGEX "${SHADER_DIR}/.*spir-v/.*") # Only include spir-v shaders

# Make output dir for embedded shader headers
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/RaeptorCogs)

set(EMBEDDED_NORMAL_SHADER_HEADER ${CMAKE_CURRENT_BINARY_DIR}/include/RaeptorCogs/EmbedShaders.hpp)
set(EMBEDDED_SPIRV_SHADER_HEADER ${CMAKE_CURRENT_BINARY_DIR}/include/RaeptorCogs/EmbedSPIRVShaders.hpp)

add_custom_command(
    OUTPUT "${EMBEDDED_NORMAL_SHADER_HEADER}"
    COMMAND $<TARGET_FILE:rgsl>
        --include "${SHADER_DIR}"
        -VC
        --embed
        --output "${EMBEDDED_NORMAL_SHADER_HEADER}"
        ${NORMAL_SHADER_SOURCES}
    DEPENDS ${NORMAL_SHADER_SOURCES} rgsl
    COMMENT "Embedding shaders into ${EMBEDDED_NORMAL_SHADER_HEADER} using rgsl"
    VERBATIM
)

add_custom_command(
    OUTPUT "${EMBEDDED_SPIRV_SHADER_HEADER}"
    COMMAND $<TARGET_FILE:rgsl>
        --include "${SHADER_DIR}"
        -VCS
        --embed
        --output "${EMBEDDED_SPIRV_SHADER_HEADER}"
        ${SPIRV_SHADER_SOURCES}
    DEPENDS ${SPIRV_SHADER_SOURCES} rgsl
    COMMENT "Embedding shaders into ${EMBEDDED_SPIRV_SHADER_HEADER} using rgsl"
    VERBATIM
)

#========================================================================#
# SOURCES
#========================================================================#

add_library(RaeptorCogs STATIC ${LIB_SOURCES})

if (TARGET_WEBASM)
  set_target_properties(RaeptorCogs PROPERTIES
      LINK_FLAGS "
      -sUSE_GLFW=3 
      -sUSE_WEBGL2=1 
      -sFULL_ES3=1 
      -sALLOW_MEMORY_GROWTH=1 
      -sASSERTIONS=1"
  )
endif()

# OpenSSL if needed
if(OpenSSL_FOUND)
    target_compile_definitions(RaeptorCogs PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
    target_link_libraries(RaeptorCogs PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

if(Vulkan_FOUND)
    message(STATUS "Vulkan found: ${Vulkan_LIBRARY}")
    target_compile_definitions(RaeptorCogs PRIVATE VULKAN_SUPPORTED)
    target_link_libraries(RaeptorCogs PUBLIC Vulkan::Vulkan)
else()
    message(STATUS "Vulkan not found. Vulkan support will be disabled.")
endif()

if (TBB_FOUND)
    target_link_libraries(RaeptorCogs PUBLIC TBB::tbb)
    target_compile_definitions(RaeptorCogs PUBLIC RAEPTOR_HAS_TBB)
else()
    message(STATUS "TBB not found, building without TBB support")
endif()

if (WAYLAND_FOUND)
    target_include_directories(RaeptorCogs PRIVATE ${WAYLAND_INCLUDE_DIRS})
    target_link_libraries(RaeptorCogs PRIVATE ${WAYLAND_LIBRARIES})
    target_compile_definitions(RaeptorCogs PUBLIC RAEPTOR_HAS_WAYLAND)
else()
    message(STATUS "Wayland not found, building without Wayland support")
endif()

# ---- Include dirs ----
target_include_directories(RaeptorCogs 
    PUBLIC
        ${stb_SOURCE_DIR}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GLAD_DIR}/include
        ${httplib_SOURCE_DIR}
        ${nfd_SOURCE_DIR}/src/include
        ${miniz_SOURCE_DIR}/source
)

#========================================================================#
# COMPILE OPTIONS
#========================================================================#

if (MSVC)
  target_compile_options(RaeptorCogs PRIVATE
      /EHsc
      /W4                    # MSVC warnings
      /clang:-Wall
      /clang:-Wextra
      /clang:-Wpedantic
      /clang:-Wconversion
  )
endif()

#========================================================================#
# LINK SHADERS
#========================================================================#

add_custom_target(embed_shaders DEPENDS "${EMBEDDED_NORMAL_SHADER_HEADER}")
add_dependencies(RaeptorCogs embed_shaders)

add_custom_target(compile_spirv_shaders DEPENDS "${EMBEDDED_SPIRV_SHADER_HEADER}")
add_dependencies(RaeptorCogs compile_spirv_shaders)

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
    )

    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${GLAD_DIR}/include
        ${glfw_SOURCE_DIR}/include
    )

    if (Vulkan_FOUND)
        target_sources(imgui PRIVATE
            ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        )
        target_include_directories(imgui PUBLIC
            ${Vulkan_INCLUDE_DIRS}
        )
    endif()

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
    message(STATUS "Adding example: ${NAME}")
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


    if (WIN32)

        add_executable(${NAME} ${EXAMPLE_CPP_SOURCES})

        #target_link_options(${NAME} PRIVATE -mwindows)
        # Windows icon via resource file
        target_sources(${NAME} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/examples/resources.rc
        )
    elseif (UNIX)

        add_executable(${NAME} ${EXAMPLE_CPP_SOURCES})

        target_compile_definitions(${NAME} PRIVATE APP_ID="${NAME}")

        # Linux desktop entry + icon
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/examples/raeptor-cogs-icon-16.png DESTINATION share/icons/hicolor/16x16/apps RENAME ${NAME}.png)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/examples/raeptor-cogs-icon-32.png DESTINATION share/icons/hicolor/32x32/apps RENAME ${NAME}.png)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/examples/raeptor-cogs-icon-48.png DESTINATION share/icons/hicolor/48x48/apps RENAME ${NAME}.png)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/examples/raeptor-cogs-icon-128.png DESTINATION share/icons/hicolor/128x128/apps RENAME ${NAME}.png)

        set(EXEC_PATH "$<TARGET_FILE:${NAME}>")

        # Create app.desktop file in build dir
        #configure_file(
        #    ${CMAKE_CURRENT_SOURCE_DIR}/examples/app.desktop.in
        #    ${CMAKE_BINARY_DIR}/${NAME}.desktop.in
        #    @ONLY
        #)

        # Create app.desktop file in build dir
        #file(GENERATE
        #    OUTPUT "${CMAKE_BINARY_DIR}/${NAME}.desktop"
        #    INPUT  "${CMAKE_BINARY_DIR}/${NAME}.desktop.in"
        #)
        
        # Install app.desktop file
        #install(FILES ${CMAKE_BINARY_DIR}/${NAME}.desktop
        #    DESTINATION share/applications)
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
        target_link_options(${NAME} PRIVATE
            -sUSE_GLFW=3
            -sUSE_WEBGL2=1
            -sFULL_ES3=1
            -sALLOW_MEMORY_GROWTH=1
            -sASSERTIONS=2
            -sSAFE_HEAP=1
        )
    endif()

    # Copy assets
    foreach(asset ${EXAMPLE_ASSETS})
        # Skip empty
        if(NOT asset)
            continue()
        endif()

        if (TARGET_WEBASM)
            target_link_options(${NAME} PRIVATE
                "--preload-file=${asset}@/assets"
            )
        else()
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
        endif()
    endforeach()
endfunction()

add_example(RaeptorCogsTestPlayground
    FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/examples/test-playground
    ASSETS
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/test-playground/assets
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/test-playground/protected
)

add_example(RaeptorCogsDemo
    FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/examples/demo
    ASSETS
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/demo/assets
)

#========================================================================#
# TESTING
#========================================================================#

enable_testing()
add_subdirectory(tests)

#========================================================================#
