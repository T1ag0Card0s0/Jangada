set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER aarch64-none-elf-gcc)
set(CMAKE_ASM_COMPILER aarch64-none-elf-gcc)
set(CMAKE_AR aarch64-none-elf-ar)
set(CMAKE_OBJCOPY aarch64-none-elf-objcopy)
set(CMAKE_SIZE aarch64-none-elf-size)

set(COMMON_FLAGS "-ffreestanding -nostdlib -fno-builtin -fno-stack-protector")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -mcpu=cortex-a53")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS} -x assembler-with-cpp -mcpu=cortex-a53")
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -Wl,--gc-sections")

