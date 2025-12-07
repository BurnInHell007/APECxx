# Audio Engine Test Suite Configuration

message(STATUS "Configuring tests...")

# ============================================================================
# Google Test Setup
# ============================================================================

# Try to find Google Test
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "Google Test not found, fetching from GitHub...")
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        GIT_SHALLOW    TRUE
    )
    
    # For Windows: Prevent overriding parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    message(STATUS "Google Test fetched successfully")
else()
    message(STATUS "Using system Google Test")
endif()

# ============================================================================
# Test Source Files
# ============================================================================

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_audio_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_sample_conversions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_wav_io.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_basic_effects.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_filters.cpp
)

# ============================================================================
# Test Executable
# ============================================================================

add_executable(audio_tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(audio_tests PRIVATE
    audio_engine_lib
    GTest::gtest_main
)

# Include directories
target_include_directories(audio_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Set output directory
set_target_properties(audio_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# ============================================================================
# Test Discovery
# ============================================================================

include(GoogleTest)
gtest_discover_tests(audio_tests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    PROPERTIES TIMEOUT 60
)

# ============================================================================
# Test Data Directory
# ============================================================================

# Create test_data directory in build output
set(TEST_DATA_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_data)
file(MAKE_DIRECTORY ${TEST_DATA_OUTPUT_DIR})

# Copy test data if exists
set(TEST_DATA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_data)
if(EXISTS ${TEST_DATA_SOURCE_DIR})
    message(STATUS "Copying test data...")
    file(GLOB TEST_DATA_FILES "${TEST_DATA_SOURCE_DIR}/*")
    foreach(TEST_FILE ${TEST_DATA_FILES})
        get_filename_component(FILE_NAME ${TEST_FILE} NAME)
        configure_file(
            ${TEST_FILE}
            ${TEST_DATA_OUTPUT_DIR}/${FILE_NAME}
            COPYONLY
        )
    endforeach()
    message(STATUS "Test data copied to ${TEST_DATA_OUTPUT_DIR}")
else()
    message(STATUS "No test_data directory found, created empty directory at ${TEST_DATA_OUTPUT_DIR}")
endif()

# ============================================================================
# Custom Test Targets
# ============================================================================

# Run tests with verbose output
add_custom_target(test_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --output-on-failure
    DEPENDS audio_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Run specific test suites
add_custom_target(test_buffer
    COMMAND audio_tests --gtest_filter=AudioBufferTest.*
    DEPENDS audio_tests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_custom_target(test_filters_only
    COMMAND audio_tests --gtest_filter=FilterTest.*
    DEPENDS audio_tests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_custom_target(test_effects_only
    COMMAND audio_tests --gtest_filter=BasicEffectsTest.*
    DEPENDS audio_tests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Generate XML report
add_custom_target(test_report
    COMMAND audio_tests --gtest_output=xml:test_results.xml
    DEPENDS audio_tests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

message(STATUS "Test configuration complete")
message(STATUS "  Test sources: ${TEST_SOURCES}")
message(STATUS "  Test output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/audio_tests")
