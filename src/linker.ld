/* linker.ld */

ENTRY(start)

SECTIONS
{
  . = 0x100000; /* Kernel starts at 1MB */

  .text :
  {
    *(.multiboot) /* Ensure multiboot section is first */
    *(.text)
  }

  .data :
  {
    *(.data)
  }

  .bss :
  {
    *(.bss)
  }

  .rodata :
  {
    *(.rodata)
  }

  .eh_frame :
  {
    *(.eh_frame)
  }

  .eh_frame_hdr :
  {
    *(.eh_frame_hdr)
  }
}