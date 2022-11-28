
.global _start
.section .text.init
_start:
    li sp, 0x8000F000
    jal program_constructor
    li sp, 0x8000F000
    jal main

.global done
done:
    add x28, a0, zero           # Get return code from kernel
__inf_loop:
    j __inf_loop                # Halts the processor with an infinite self loop
