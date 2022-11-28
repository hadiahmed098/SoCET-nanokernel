
.global _start
.section .text.init
_start:
    li sp, 0x8000F000
    jal program_constructor
    li sp, 0x8000F000
    jal main

.global done
done:
    addi x28, zero, 1           # Sets register 28 to 1 to pass a test bench
__inf_loop:
    j __inf_loop                # Halts the processor with an infinite self loop
 