cmake_minimum_required(VERSION 4.2.0)

project(APECxx)

# --- Define your executables ---
# this will find all .cpp files in src and store them in var SOURCE
file(GLOB SOURCES "src/*.cpp")

# Final executables
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Linking Include directory ---
# this tells include folder to be added to search path
target_include_directories(${PROJECT_NAME} PUBLIC include)
