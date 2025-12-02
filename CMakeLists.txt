cmake_minimum_required(VERSION 4.0.0)

set(This APECxx)

project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(googletest)

set(Headers 
    "include/"
    "include/WavIO/"
    "include/Effects/"
)

# file(GLOB Sources "src/*.cpp")
set(Sources 
    "src/main.cpp"
    "src/AudioBuffer.cpp"
    "src/WavIO/WavReader.cpp"
    "src/WavIO/WavWriter.cpp"
    "src/Effects/GainEffect.cpp"
    "src/Effects/FadeEffect.cpp"
    "src/Effects/MixEffect.cpp"
)

add_library(${This} ${Sources} ${Header})

add_subdirectory(tests)

target_include_directories(${This} PUBLIC ${Headers})

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
