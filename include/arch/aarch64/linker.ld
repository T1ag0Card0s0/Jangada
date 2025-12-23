
/* link.ld - aarch64 bare-metal for QEMU 'virt' */

/* ===================== */
/* Configurable Settings */
/* ===================== */
RAM_ORIGIN      = 0x40000000;  /* Default QEMU RAM base */
RAM_SIZE        = 128M;         /* Default RAM size */
STACK_SIZE      = 64K;          /* Stack size at top of RAM */
VECTOR_ALIGN    = 2048;         /* Exception vector alignment */
PAGE_ALIGN      = 4096;         /* Page alignment for code/data */
INIT_FINI_ALIGN = 8;            /* Alignment for init/fini arrays */
BSS_ALIGN       = 16;           /* Alignment for BSS */

/* ================== */
/* Memory Declaration */
/* ================== */
MEMORY
{
    RAM (rwx) : ORIGIN = RAM_ORIGIN, LENGTH = RAM_SIZE
}

/* Entry point */
ENTRY(_start)

/* Useful global symbols */
_ram_origin_ = ORIGIN(RAM);
_ram_end_    = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    /* --------------------------- */
    /* Exception vector table      */
    /* --------------------------- */
    . = ORIGIN(RAM);
    . = ALIGN(VECTOR_ALIGN);
    __vectors_start = .;
    .vectors : ALIGN(VECTOR_ALIGN)
    {
        KEEP(*(.vectors))
    } > RAM
    __vectors_end = .;

    /* --------------------------- */
    /* Code section (.text)        */
    /* --------------------------- */
    . = ALIGN(PAGE_ALIGN);
    __text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
    } > RAM
    __text_end = .;

    /* --------------------------- */
    /* Read-only data (.rodata)    */
    /* --------------------------- */
    . = ALIGN(PAGE_ALIGN);
    __rodata_start = .;
    .rodata :
    {
        *(.rodata) 
        *(.rodata.*)
        *(.gnu.linkonce.r.*)
    } > RAM
    __rodata_end = .;

    /* --------------------------- */
    /* C++ constructors/destructors */
    /* --------------------------- */
    . = ALIGN(INIT_FINI_ALIGN);
    __init_array_start = .;
    .init_array :
    {
        KEEP(*(.init_array))
        KEEP(*(.init_array.*))
    } > RAM
    __init_array_end = .;

    . = ALIGN(INIT_FINI_ALIGN);
    __fini_array_start = .;
    .fini_array :
    {
        KEEP(*(.fini_array))
        KEEP(*(.fini_array.*))
    } > RAM
    __fini_array_end = .;

    /* --------------------------- */
    /* Writable data (.data)       */
    /* --------------------------- */
    . = ALIGN(PAGE_ALIGN);
    __data_start = .;
    .data :
    {
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
    } > RAM
    __data_end = .;

    /* --------------------------- */
    /* Zero-initialized data (.bss) */
    /* --------------------------- */
    . = ALIGN(BSS_ALIGN);
    _bss_start_ = .;
    .bss (NOLOAD) :
    {
        *(.bss) 
        *(.bss.*)
        *(COMMON)
    } > RAM
    _bss_end_ = .;

    /* --------------------------- */
    /* Heap (optional)             */
    /* --------------------------- */
    . = ALIGN(BSS_ALIGN);
    _heap_start_ = .;

    /* --------------------------- */
    /* Stack at top of RAM          */
    /* --------------------------- */
    _stack_size_   = STACK_SIZE;
    _stack_top_    = ORIGIN(RAM) + LENGTH(RAM);
    _stack_bottom_ = _stack_top_ - _stack_size_;

    /* Ensure stack does not overlap image */
    ASSERT(_bss_end_ < _stack_bottom_, "Image overlaps stack!")

    /* Provide standard symbols */
    PROVIDE(_end_ = _bss_end_);
    PROVIDE(end = _bss_end_);
}

/* --------------------------- */
/* Sanity checks               */
/* --------------------------- */
ASSERT((__vectors_start & (VECTOR_ALIGN - 1)) == 0, "VBAR_EL1 must be 2KB aligned");

