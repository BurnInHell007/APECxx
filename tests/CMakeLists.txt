cmake_minimum_required(VERSION 4.0.0)

set(This Tests)

set(Sources
    "test_audio_buffer.cpp"
    "test_wav_io.cpp"
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
    gtest_main
    APECxx
)

target_include_directories(${This} PRIVATE
    ../include
)

add_test(
    NAME ${This}
    COMMAND ${This}
)

