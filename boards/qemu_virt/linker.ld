/* link.ld - aarch64 bare-metal for QEMU 'virt' */
/* QEMU 'virt' default RAM base */
MEMORY
{
    RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 128M
}

/* Entry symbol */
ENTRY(_start)

/* Expose useful symbols */
_ram_origin_ = ORIGIN(RAM);
_ram_end_ = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    /* Exception vector table: must be 2KB aligned for VBAR_EL1 */
    . = ORIGIN(RAM);
    . = ALIGN(2048);
    __vectors_start = .;
    .vectors : ALIGN(2048)
    {
        KEEP(*(.vectors))
    } > RAM
    __vectors_end = .;

    /* Put code right after vectors, page-align for neatness */
    . = ALIGN(4096);
    __text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
    } > RAM
    __text_end = .;

    /* Read-only data */
    . = ALIGN(4096);
    __rodata_start = .;
    .rodata :
    {
        *(.rodata) 
        *(.rodata.*)
        *(.gnu.linkonce.r.*)
    } > RAM
    __rodata_end = .;

    /* C++ constructors/destructors */
    . = ALIGN(8);
    __init_array_start = .;
    .init_array :
    {
        KEEP(*(.init_array))
        KEEP(*(.init_array.*))
    } > RAM
    __init_array_end = .;

    . = ALIGN(8);
    __fini_array_start = .;
    .fini_array :
    {
        KEEP(*(.fini_array))
        KEEP(*(.fini_array.*))
    } > RAM
    __fini_array_end = .;

    /* Writable data */
    . = ALIGN(4096);
    __data_start = .;
    .data :
    {
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
    } > RAM
    __data_end = .;

    /* Zero-initialized data (won't be in the image) */
    . = ALIGN(16);
    _bss_start_ = .;
    .bss (NOLOAD) :
    {
        *(.bss) 
        *(.bss.*)
        *(COMMON)
    } > RAM
    _bss_end_ = .;

    /* Optional: a simple stack near top of RAM (adjust size as needed) */
    . = ALIGN(16);
    _heap_start_ = .;

    /* Define stack at the top of RAM. Startup can set SP = _stack_top_. */
    _stack_size_ = 64K;
    _stack_top_  = ORIGIN(RAM) + LENGTH(RAM);
    _stack_bottom_ = _stack_top_ - _stack_size_;

    /* Sanity: keep image well below stack bottom */
    ASSERT(_bss_end_ < _stack_bottom_, "Image overlaps stack!")

    /* Provide symbols that many runtime stubs expect */
    PROVIDE(_end_ = _bss_end_);
    PROVIDE(end = _bss_end_);
}

/* Extra safety: make sure vectors are 2KB-aligned */
ASSERT((__vectors_start & 0x7FF) == 0, "VBAR_EL1 must be 2KB aligned");
