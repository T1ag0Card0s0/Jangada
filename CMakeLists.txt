cmake_minimum_required(VERSION 3.20)

project(jangada C CXX ASM)

message(STATUS "Building for ${BOARD} ${ARCH}")
message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(boards/${BOARD})
add_subdirectory(arch/${ARCH})
add_subdirectory(kernel)

add_library(jangada
    ${KERNEL_SOURCES}
    ${ARCH_SOURCES}
)

target_link_options(jangada INTERFACE "-T${BOARD_LINKER_SCRIPT}")
