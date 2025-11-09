cmake_minimum_required(VERSION 3.15)

project(AudioEngine)

set (CMAKE_CXX_COMPILER g++)

# set c++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# options
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(ENABLE_ASAN "Enable Address Sanitizer (Debug)" OFF)

# Compiler flags
if(ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4 /WX)
    else()
        add_compile_options(
            -Wextra 
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
        )
    endif()
endif()

# Address Sanitizer (for debugging memory issues)
if(ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(NOT MSVC)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(AUDIO_ENGINE_SOURCES
    src/wav_reader.cpp
    src/wav_writer.cpp
)

set(AUDIO_ENGINE_HEADERS
    include/audio_buffer.hpp
    include/wav_reader.hpp
    include/wav_writer.hpp
)

# Create a library (reusable in tests and main executable)
add_library(audio_engine_lib STATIC
    ${AUDIO_ENGINE_SOURCES}
    ${AUDIO_ENGINE_HEADERS}
)

target_include_directories(audio_engine_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Main executable
add_executable(audio_tool
    src/main.cpp
)

target_link_libraries(audio_tool PRIVATE audio_engine_lib)

# Windows-specific: Ensure static linking for MinGW
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(audio_tool PRIVATE -static-libgcc -static-libstdc++)
endif()

# Installation rules
install(TARGETS audio_tool
    RUNTIME DESTINATION bin
)

install(TARGETS audio_engine_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Print configuration
message(STATUS "")
message(STATUS "AudioEngine Configuration:")
message(STATUS "  C++ Standard:        ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type:          ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build Tests:         ${BUILD_TESTS}")
message(STATUS "  Enable Warnings:     ${ENABLE_WARNINGS}")
message(STATUS "  Enable ASAN:         ${ENABLE_ASAN}")
message(STATUS "  Install Prefix:      ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

# Generate compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
