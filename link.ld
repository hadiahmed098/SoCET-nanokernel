OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x0000000080000000;
  .text.init : { *(.text.init) }
  .text : { *(.text) }
  .data ALIGN(0x1000) : { *(.data) }
  .bss : { *(.bss) }
  . = 0x0000000080007000;
  .statuses : { *(.statuses) }
  .userprog :
  {
    _suserprog = .;
    *(.userprog.*)
    _euserprog = .;
  }
  _end = .;
}
