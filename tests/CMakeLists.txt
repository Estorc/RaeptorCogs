cmake_policy(SET CMP0135 NEW)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(RaeptorCogs_tests ${TEST_SOURCES})

target_compile_features(RaeptorCogs_tests PRIVATE cxx_std_17)

set_target_properties(RaeptorCogs_tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(RaeptorCogs_tests
    PRIVATE
        RaeptorCogs
        GTest::gtest_main
)

# Disable raw asserts in tests
target_compile_definitions(RaeptorCogs_tests PRIVATE NDEBUG)

# Warnings level
target_compile_options(RaeptorCogs_tests
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

# Silence GTest warnings
if(MSVC)
    foreach(target gtest gtest_main gmock gmock_main)
        if (TARGET ${target})
            target_compile_options(${target} PRIVATE /W0)
        endif()
    endforeach()
else()
    foreach(target gtest gtest_main)
        if (TARGET ${target})
            target_compile_options(${target} PRIVATE -w)
        endif()
    endforeach()
endif()

# Disable all warnings in test target
target_compile_options(RaeptorCogs_tests PRIVATE
    -w                                      # Clang / GCC: disable all warnings
    $<$<CXX_COMPILER_ID:MSVC>:/W0>          # MSVC: disable warnings
)

include(GoogleTest)
gtest_discover_tests(RaeptorCogs_tests)
