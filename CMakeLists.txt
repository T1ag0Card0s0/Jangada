cmake_minimum_required(VERSION 3.16)
project(JangadaRTOS LANGUAGES C ASM)

add_executable(jangada ${SOURCES})

target_include_directories(jangada PRIVATE ${CMAKE_BINARY_DIR})

