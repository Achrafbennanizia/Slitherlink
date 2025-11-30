cmake_minimum_required(VERSION 3.16)

# -------------------------------------------------------
# Project
# -------------------------------------------------------
project(SlitherlinkSolver
        VERSION 0.1.0
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
option(SLITHERLINK_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(SLITHERLINK_ENABLE_SANITIZERS "Enable address/UB sanitizers (Debug only, GCC/Clang)" OFF)

# -------------------------------------------------------
# Executable
# -------------------------------------------------------
add_executable(slitherlink
        main.cpp
)

# -------------------------------------------------------
# Threading (for std::async / std::thread)
# -------------------------------------------------------
find_package(Threads REQUIRED)
target_link_libraries(slitherlink PRIVATE Threads::Threads)

# -------------------------------------------------------
# Intel oneAPI TBB (Threading Building Blocks)
# -------------------------------------------------------
find_package(TBB QUIET)
if(TBB_FOUND)
    message(STATUS "Found Intel TBB: ${TBB_VERSION}")
    target_link_libraries(slitherlink PRIVATE TBB::tbb)
    target_compile_definitions(slitherlink PRIVATE USE_TBB)
else()
    message(WARNING "Intel TBB not found. Install with: brew install tbb (macOS)")
endif()

# -------------------------------------------------------
# Google OR-Tools (Constraint Programming)
# -------------------------------------------------------
# Find all required dependencies for OR-Tools
find_library(ORTOOLS_LIBRARY NAMES ortools PATHS /opt/homebrew/lib /usr/local/lib)
find_library(PROTOBUF_LIBRARY NAMES protobuf PATHS /opt/homebrew/lib /usr/local/lib)
find_library(ABSL_BASE_LIBRARY NAMES absl_base PATHS /opt/homebrew/lib /usr/local/lib)
find_library(ABSL_STRINGS_LIBRARY NAMES absl_strings PATHS /opt/homebrew/lib /usr/local/lib)
find_path(ORTOOLS_INCLUDE_DIR NAMES ortools/sat/cp_model.h PATHS /opt/homebrew/include /usr/local/include)

if(ORTOOLS_LIBRARY AND PROTOBUF_LIBRARY AND ABSL_BASE_LIBRARY AND ORTOOLS_INCLUDE_DIR)
    message(STATUS "Found OR-Tools: ${ORTOOLS_LIBRARY}")
    message(STATUS "Found Protobuf: ${PROTOBUF_LIBRARY}")
    message(STATUS "Found Abseil: ${ABSL_BASE_LIBRARY}")
    
    target_include_directories(slitherlink PRIVATE 
        ${ORTOOLS_INCLUDE_DIR}
        /opt/homebrew/include
    )
    
    # Link all required libraries
    target_link_libraries(slitherlink PRIVATE 
        ${ORTOOLS_LIBRARY}
        ${PROTOBUF_LIBRARY}
        ${ABSL_BASE_LIBRARY}
        ${ABSL_STRINGS_LIBRARY}
    )
    
    target_compile_definitions(slitherlink PRIVATE USE_ORTOOLS)
else()
    message(WARNING "OR-Tools dependencies not found. Install with: brew install or-tools protobuf abseil")
endif()

# -------------------------------------------------------
# Warnings
# -------------------------------------------------------
if (MSVC)
    target_compile_options(slitherlink PRIVATE /W4)
    if(SLITHERLINK_WARNINGS_AS_ERRORS)
        target_compile_options(slitherlink PRIVATE /WX)
    endif()
else()
    target_compile_options(slitherlink PRIVATE -Wall -Wextra -Wpedantic)
    if(SLITHERLINK_WARNINGS_AS_ERRORS)
        target_compile_options(slitherlink PRIVATE -Werror)
    endif()
endif()

# -------------------------------------------------------
# Sanitizers (optional, good for debugging on server/local)
# -------------------------------------------------------
if(SLITHERLINK_ENABLE_SANITIZERS AND NOT MSVC)
    # Only make sense in Debug or RelWithDebInfo usually
    if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
        message(STATUS "Enabling Address/UB sanitizers")
        target_compile_options(slitherlink PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )
        target_link_options(slitherlink PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )
    endif()
endif()
