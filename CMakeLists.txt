cmake_minimum_required(VERSION 4.2.0)

set(This APECxx)

project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(googletest)

set(Headers 
    "include/audio_buffer.hpp"
    "include/wav_reader.hpp"
    "include/wav_writer.hpp"
    "include/project.h"
)

# file(GLOB Sources "src/*.cpp")
set(Sources 
    "src/main.cpp"
    "src/audio_buffer.cpp"
    "src/wav_reader.cpp"
    "src/wav_writer.cpp"
)

add_library(${This} ${Sources} ${Header})

add_subdirectory(tests)

target_include_directories(${This} PUBLIC "include")

add_executable(${This}_Run ${Sources})

target_link_libraries(${This}_Run PRIVATE ${This})

message(STATUS "")
message(STATUS "Building ${This} library")
message(STATUS "")
message(STATUS "Audio Processing Engine Config")
message(STATUS "    C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "    C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "    Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "Build Configuration")
message(STATUS "    Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "    Build Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "    Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "")
