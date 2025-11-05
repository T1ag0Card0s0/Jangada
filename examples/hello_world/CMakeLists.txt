cmake_minimum_required(VERSION 3.15)
project(hello_world C ASM)

set(Jangada_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)

find_package(Jangada REQUIRED)

add_executable(hello_world.elf src/main.c)

target_link_libraries(hello_world.elf PRIVATE Jangada::kernel)

