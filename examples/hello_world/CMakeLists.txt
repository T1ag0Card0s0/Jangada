cmake_minimum_required(VERSION 3.15)
project(hello_world)

set(JANGADA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../)

add_subdirectory(JANGADA_ROOT)

add_executable(hello_world src/main.c)

target_link_libraries(hello_world jangada)
