#include <stdint.h>
#include "lib_internal.h"

extern uint32_t _suserprog;
extern uint32_t _euserprog;

void main(void)
{
    // Preserve the return address
    uint32_t curr_ra;
    asm volatile("add %0, ra, zero" : "=r"(curr_ra));
    asm volatile("csrw mscratch, %0" : : "r"(curr_ra));

    // Create a new stack for the user programs
    uint32_t curr_sp = 0x2007FFF;

    // Setup the interrupt handler
    uint32_t mtvec_value = (uint32_t) handler;
    mtvec_value |= 1; // set to vectored mode
    asm volatile("csrw mtvec, %0" : : "r"(mtvec_value));
    // Enable all interrupts
    uint32_t mie_value = 0x888;
    asm volatile("csrw mie, %0" : : "r"(mie_value));
    // Globally enable interrupts
    uint32_t mstatus_value = 0x80;
    asm volatile("csrs mstatus, %0" : : "r" (mstatus_value));
    // Allow program execution with PMP
    asm volatile("csrw pmpaddr0, %0" : : "r"(((uint32_t) &_suserprog) >> 2));
    asm volatile("csrw pmpaddr1, %0" : : "r"(((uint32_t) ((&_euserprog) + 1)) >> 2));
    asm volatile("csrw pmpaddr2, %0" : : "r"(curr_sp));
    uint32_t pmp_cfg = 0x1B0D00;
    asm volatile("csrw pmpcfg0, %0" : : "r"(pmp_cfg));

    // Jump into the user program
    sys_yield();

    __builtin_unreachable();
}
