cmake_minimum_required(VERSION 3.16)

# -------------------------------------------------------
# Project
# -------------------------------------------------------
project(SlitherlinkSolver
        VERSION 0.2.0
        DESCRIPTION "High-performance Slitherlink puzzle solver"
        LANGUAGES CXX
)

# -------------------------------------------------------
# C++ standard
# -------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json (nice for tooling, clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------------------------------------
# Default build type for single-config generators (Makefile, Ninja)
# (CLion / multi-config generators like Xcode/MSVC will ignore this)
# -------------------------------------------------------
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# -------------------------------------------------------
# Options
# -------------------------------------------------------
option(SLITHERLINK_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(SLITHERLINK_BUILD_TESTS "Build unit tests" OFF)
option(SLITHERLINK_BUILD_EXAMPLES "Build example programs" ON)
option(SLITHERLINK_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(SLITHERLINK_ENABLE_SANITIZERS "Enable address/UB sanitizers (Debug only, GCC/Clang)" OFF)

# -------------------------------------------------------
# Library Target (Reusable Core - SOLID Architecture)
# -------------------------------------------------------
# Note: Current main.cpp is monolithic and doesn't use these sources
# This library is for future modular usage and examples

# Dummy source to satisfy CMake (library infrastructure reserved for future)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "// Placeholder for future library sources\n")
set(SLITHERLINK_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

# -------------------------------------------------------
# Library Target (Reusable Core - SOLID Architecture)
# -------------------------------------------------------
# Note: Current main.cpp is monolithic and doesn't use these sources
# This library section kept for future modular architecture
if(SLITHERLINK_BUILD_SHARED_LIBS)
    add_library(slitherlink_lib SHARED ${SLITHERLINK_SOURCES})
else()
    add_library(slitherlink_lib STATIC ${SLITHERLINK_SOURCES})
endif()

target_include_directories(slitherlink_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(slitherlink_lib PROPERTIES
    OUTPUT_NAME slitherlink
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXCLUDE_FROM_ALL TRUE  # Don't build by default since not used yet
)

# -------------------------------------------------------
# Executable (CLI Application) - Monolithic
# -------------------------------------------------------
add_executable(slitherlink
        apps/slitherlink_cli/main.cpp
)

# Main executable doesn't link library - it's self-contained
# target_link_libraries(slitherlink PRIVATE slitherlink_lib)

# -------------------------------------------------------
# Optimization Flags
# -------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(slitherlink PRIVATE
            -O3                    # Maximum optimization
            -march=native          # Use CPU-specific instructions
            -funroll-loops         # Unroll loops for better performance
            -ffast-math            # Aggressive floating-point optimizations
        )
        # Link-time optimization
        if(NOT APPLE)  # LTO can be problematic on macOS
            set_target_properties(slitherlink PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION TRUE
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(slitherlink PRIVATE /O2 /GL)
    endif()
endif()

# -------------------------------------------------------
# Threading (for std::async / std::thread)
# -------------------------------------------------------
find_package(Threads REQUIRED)
target_link_libraries(slitherlink PUBLIC Threads::Threads)

# -------------------------------------------------------
# Intel oneAPI TBB (Threading Building Blocks)
# -------------------------------------------------------
find_package(TBB QUIET)
if(TBB_FOUND)
    message(STATUS "Found Intel TBB: ${TBB_VERSION}")
    target_link_libraries(slitherlink PUBLIC TBB::tbb)
    target_compile_definitions(slitherlink PUBLIC USE_TBB)
else()
    message(WARNING "Intel TBB not found. Install with: brew install tbb (macOS)")
endif()

# -------------------------------------------------------
# Google OR-Tools (Constraint Programming) - Optional
# -------------------------------------------------------
find_library(ORTOOLS_LIBRARY NAMES ortools PATHS /opt/homebrew/lib /usr/local/lib)
find_library(PROTOBUF_LIBRARY NAMES protobuf PATHS /opt/homebrew/lib /usr/local/lib)
find_library(ABSL_BASE_LIBRARY NAMES absl_base PATHS /opt/homebrew/lib /usr/local/lib)
find_library(ABSL_STRINGS_LIBRARY NAMES absl_strings PATHS /opt/homebrew/lib /usr/local/lib)
find_path(ORTOOLS_INCLUDE_DIR NAMES ortools/sat/cp_model.h PATHS /opt/homebrew/include /usr/local/include)

if(ORTOOLS_LIBRARY AND PROTOBUF_LIBRARY AND ABSL_BASE_LIBRARY AND ORTOOLS_INCLUDE_DIR)
    message(STATUS "Found OR-Tools: ${ORTOOLS_LIBRARY}")
    message(STATUS "Found Protobuf: ${PROTOBUF_LIBRARY}")
    message(STATUS "Found Abseil: ${ABSL_BASE_LIBRARY}")
    
    target_include_directories(slitherlink_lib PUBLIC
        ${ORTOOLS_INCLUDE_DIR}
        /opt/homebrew/include
    )
    
    target_link_libraries(slitherlink_lib PUBLIC
        ${ORTOOLS_LIBRARY}
        ${PROTOBUF_LIBRARY}
        ${ABSL_BASE_LIBRARY}
        ${ABSL_STRINGS_LIBRARY}
    )
    
    target_compile_definitions(slitherlink_lib PUBLIC USE_ORTOOLS)
else()
    message(WARNING "OR-Tools dependencies not found. Install with: brew install or-tools protobuf abseil")
endif()

# -------------------------------------------------------
# Compiler Warnings
# -------------------------------------------------------
if (MSVC)
    target_compile_options(slitherlink_lib PRIVATE /W4)
    if(SLITHERLINK_WARNINGS_AS_ERRORS)
        target_compile_options(slitherlink_lib PRIVATE /WX)
    endif()
else()
    target_compile_options(slitherlink_lib PRIVATE -Wall -Wextra -Wpedantic)
    if(SLITHERLINK_WARNINGS_AS_ERRORS)
        target_compile_options(slitherlink_lib PRIVATE -Werror)
    endif()
endif()

# -------------------------------------------------------
# Sanitizers (optional, good for debugging on server/local)
# -------------------------------------------------------
if(SLITHERLINK_ENABLE_SANITIZERS AND NOT MSVC)
    if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
        message(STATUS "Enabling Address/UB sanitizers")
        target_compile_options(slitherlink_lib PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )
        target_link_options(slitherlink_lib PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )
    endif()
endif()

# -------------------------------------------------------
# Installation
# -------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS slitherlink_lib slitherlink
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/slitherlink
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# -------------------------------------------------------
# Testing (optional)
# -------------------------------------------------------
if(SLITHERLINK_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# -------------------------------------------------------
# Examples (optional)
# -------------------------------------------------------
if(SLITHERLINK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# -------------------------------------------------------
# Print Configuration Summary
# -------------------------------------------------------
message(STATUS "")
message(STATUS "====== Slitherlink Solver Configuration ======")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Shared Libraries: ${SLITHERLINK_BUILD_SHARED_LIBS}")
message(STATUS "Build Tests: ${SLITHERLINK_BUILD_TESTS}")
message(STATUS "TBB Support: ${TBB_FOUND}")
message(STATUS "OR-Tools Support: ${ORTOOLS_LIBRARY}")
message(STATUS "==============================================")
message(STATUS "")
