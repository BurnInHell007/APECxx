cmake_minimum_required(VERSION 3.15)

# Project definition
project(AudioProcessingEngine 
    VERSION 1.0.0
    DESCRIPTION "Real-time audio processing engine with DSP filters"
    LANGUAGES CXX
)

# Require C++17
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTS "Build the test suite" ON)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(ENABLE_ASAN "Enable AddressSanitizer (Debug builds only)" OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Compiler flags
# ============================================================================

if(ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
endif()

# Address Sanitizer (for debugging memory issues)
if(ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# Windows-specific: Static linking for MinGW
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

# ============================================================================
# Include directories
# ============================================================================

include_directories(${CMAKE_SOURCE_DIR}/include)

# ============================================================================
# Source files
# ============================================================================

# Header files (for IDE integration)
set(AUDIO_ENGINE_HEADERS
    # Core
    include/project.h
    include/AudioBuffer.hpp
    include/SampleConversion.hpp
    
    # WAV I/O
    include/WavIO/WavReader.hpp
    include/WavIO/WavWriter.hpp
    
    # DSP
    include/DSP/BiQuadFilter.hpp
    include/DSP/FilterDesign.hpp
    
    # Effects
    include/Effects/AudioEffect.hpp
    include/Effects/BasicEffects.hpp
    include/Effects/FilterEffects.hpp
    include/Effects/Equalizer.hpp
)

# Source files
set(AUDIO_ENGINE_SOURCES
    src/WavIO/WavReader.cpp
    src/WavIO/WavWriter.cpp
)

# ============================================================================
# Audio Engine Library
# ============================================================================

add_library(audio_engine_lib STATIC
    ${AUDIO_ENGINE_SOURCES}
    ${AUDIO_ENGINE_HEADERS}
)

set_target_properties(audio_engine_lib PROPERTIES
    OUTPUT_NAME "audio_engine"
)

target_include_directories(audio_engine_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# ============================================================================
# Main executable - audio_tool
# ============================================================================

add_executable(audio_tool
    src/main.cpp
)

target_link_libraries(audio_tool PRIVATE
    audio_engine_lib
)

# ============================================================================
# Testing
# ============================================================================

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# Installation
# ============================================================================

install(TARGETS audio_tool
    RUNTIME DESTINATION bin
)

install(TARGETS audio_engine_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/audio_engine
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# ============================================================================
# Export compile commands (for IDEs)
# ============================================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Configuration summary
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "Audio Processing Engine Configuration")
message(STATUS "========================================")
message(STATUS "Version:             ${PROJECT_VERSION}")
message(STATUS "C++ Standard:        C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler:            ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "")
message(STATUS "Build Options:")
message(STATUS "  Build Tests:       ${BUILD_TESTS}")
message(STATUS "  Enable Warnings:   ${ENABLE_WARNINGS}")
message(STATUS "  AddressSanitizer:  ${ENABLE_ASAN}")
message(STATUS "")
message(STATUS "Output Directories:")
message(STATUS "  Runtime:           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "  Library:           ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "========================================")
message(STATUS "")
