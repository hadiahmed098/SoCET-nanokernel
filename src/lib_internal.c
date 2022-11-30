#include "lib_internal.h"
#include "progs/lib.h"
#include "lib_common.h"
#include <stdint.h>

#define MTIME_ADDR      0xFFFFFFE0
#define MTIMEH_ADDR     0xFFFFFFE4
#define MTIMECMP_ADDR   0xFFFFFFE8
#define MTIMECMPH_ADDR  0xFFFFFFEC
#define MSIP_ADDR       0xFFFFFFF0
#define EXT_ADDR_SET    0xFFFFFFF4
#define EXT_ADDR_CLEAR  0xFFFFFFF8
#define MAGIC_ADDR      0xFFFFFFFC

typedef struct {
    int ex_code : 31;
    int interrupt : 1;
} mcause_t;

// The parameter is the exit code, '1' is success
static void end_kernel(uint8_t);

#include "progs/prog1.h"
#include "progs/prog2.h"
#include "progs/prog3.h"
#include "progs/prog4.h"

// Array of function pointers for the 'main' entry points
#define NUM_PROGS 4
int cur_prog;
void (*progs[NUM_PROGS]) (void);

void program_constructor(void)
{
    cur_prog = -1;
    // Initialize the array here
    progs[0] = &prog1_main;
    progs[1] = &prog2_main;
    progs[2] = &prog3_main;
    progs[3] = &prog4_main;
}

// Main interrupt handler
void __attribute__((interrupt)) int_handler()
{
    mcause_t mcause;
    uint32_t mie, mip, mstatus;
    asm volatile("csrr %0, mcause" : "=r"(mcause));
    asm volatile("csrr %0, mie" : "=r"(mie));
    asm volatile("csrr %0, mip" : "=r"(mip));
    asm volatile("csrr %0, mstatus" : "=r"(mstatus));

    sys_print_str("\n[kern]: interrupt handled:\n");
    sys_print_str("[kern]: currently executing program ");
    sys_print_hex(cur_prog + 1);
    sys_print_str("\n");

    sys_print_str("[kern]: mstatus: ");
    sys_print_hex(mstatus);
    sys_print_str("\n");

    sys_print_str("[kern]: mcause: ");
    sys_print_hex(mcause.interrupt & 0x1);
    sys_print_str(" ");
    sys_print_hex(mcause.ex_code);
    sys_print_str("\n");

    sys_print_str("[kern]: mie: ");
    sys_print_hex(mie);
    sys_print_str("\n");

    sys_print_str("[kern]: mip: ");
    sys_print_hex(mip);
    sys_print_str("\n\n");

    asm volatile("csrw mip, zero"); // Just destroy every pending interrupt
    volatile uint32_t *mtimecmph = (volatile uint32_t*) MTIMECMPH_ADDR;
    *mtimecmph = 0x1 << 30; // Set some very large number
}

// Main exception handler
void __attribute__((interrupt)) ex_handler()
{
    uint32_t type;
    void *arg;
    asm volatile("add %0, a0, zero" : "=r" (type));
    asm volatile("add %0, a1, zero" : "=r" (arg));

    uint32_t mepc, mtval, mstatus;
    mcause_t mcause;
    asm volatile("csrr %0, mepc" : "=r"(mepc));
    asm volatile("csrr %0, mtval" : "=r"(mtval));
    asm volatile("csrr %0, mcause" : "=r"(mcause));
    asm volatile("csrr %0, mstatus": "=r"(mstatus));

    if ((mstatus & 0x1800) == 0x1800) // The kernel was in machine mode when an exception occured
    {
        sys_print_str("\n\n[kern]: FATAL ERROR: KERNEL PANIC\n");
        sys_print_str("[kern]: Exception occured while handling an interrupt or exception\n");
        sys_print_str("[kern]: mepc: ");
        sys_print_hex(mepc);
        sys_print_str("\n");

        sys_print_str("[kern]: mtval: ");
        sys_print_hex(mtval);
        sys_print_str("\n");

        sys_print_str("[kern]: mcause: ");
        sys_print_hex(mcause.interrupt & 0x1);
        sys_print_str(" ");
        sys_print_hex(mcause.ex_code);
        sys_print_str("\n");

        end_kernel(0);
        __builtin_unreachable();
    }

    if (mcause.ex_code == 0x8)
    {
        switch (type)
        {
        case PRINT_STR:
            sys_print_str(arg);
            break;

        case PRINT_HEX:
            sys_print_hex(*((uint32_t*)arg));
            break;

        case YIELD:
            sys_yield();
            __builtin_unreachable(); // yield should never come back to here
            break;

        case TIME:
            sys_time(*((uint16_t*)arg));
            break;

        default:
            sys_print_str("\n[kern]: unknown request: ");
            sys_print_hex(type);
            sys_print_str(" ");
            sys_print_hex((uint32_t) arg);
            sys_print_str("\n\n");
            break;
        }

        mepc += 4;

        asm volatile("csrw mepc, %0" : : "r"(mepc));
    }
    else
    {
        sys_print_str("\n[kern]: PROGRAM ERROR:\n");
        sys_print_str("[kern]: exception in program ");
        sys_print_hex(cur_prog + 1);
        sys_print_str("\n");

        sys_print_str("[kern]: mepc: ");
        sys_print_hex(mepc);
        sys_print_str("\n");

        sys_print_str("[kern]: mtval: ");
        sys_print_hex(mtval);
        sys_print_str("\n");

        sys_print_str("[kern]: mcause: ");
        sys_print_hex(mcause.interrupt & 0x1);
        sys_print_str(" ");
        sys_print_hex(mcause.ex_code);
        sys_print_str("\n");

        sys_yield();
        __builtin_unreachable();
    }
}

/*
 *  RISC-V Vector Layout
 *  0 - exception (overlap of interrupt cause & exception cause)
 *  1 - S-SW
 *  2 - reserved
 *  3 - M-SW
 *  4 - reserved
 *  5 - S-Timer
 *  6 - reserved
 *  7 - M-Timer
 *  8 - reserved
 *  9 - S-Ext
 *  10- reserved
 *  11- M-Ext
 */
void __attribute__((naked)) __attribute__((aligned(4))) handler() {
    asm volatile(".align 2; j ex_handler"); // 0
    asm volatile(".align 2; j int_handler");     // 1
    asm volatile(".align 2; j int_handler");     // 2
    asm volatile(".align 2; j int_handler");     // 3
    asm volatile(".align 2; j int_handler");     // 4
    asm volatile(".align 2; j int_handler");     // 5
    asm volatile(".align 2; j int_handler");     // 6
    asm volatile(".align 2; j int_handler");     // 7
    asm volatile(".align 2; j int_handler");     // 8
    asm volatile(".align 2; j int_handler");     // 9
    asm volatile(".align 2; j int_handler");     // 10
    asm volatile(".align 2; j int_handler");     // 11
}

/************ Library functions ************/

void sys_print_str(char *string)
{
    volatile char *magic = (volatile char *)MAGIC_ADDR;

    for(int i = 0; string[i]; i++) {
        (*magic) = string[i];
    }
}

void sys_print_hex(uint32_t x)
{
    char buf[11] = {0};
    buf[0] = '0';
    buf[1] = 'x';

    for(int i = 2; i < 10; i++) {
        uint8_t value = (x & 0xF);
        if(value >= 10) {
            buf[11-i] = ((value-10) + 'A');
        } else {
            buf[11-i] = (value + '0');
        }
        x >>= 4;
    }
    sys_print_str(buf);
}

void sys_time(uint16_t interval)
{
    volatile uint32_t *mtime = (volatile uint32_t*) MTIME_ADDR;
    volatile uint32_t *mtimeh = (volatile uint32_t*) MTIMEH_ADDR;
    volatile uint32_t *mtimecmp = (volatile uint32_t*) MTIMECMP_ADDR;
    volatile uint32_t *mtimecmph = (volatile uint32_t*) MTIMECMPH_ADDR;

    uint64_t time = ((uint64_t) *mtimeh << 32) | (*mtime);
    time += interval;
    *mtimecmph = time >> 32;
    *mtimecmp = time & UINT32_MAX;
}

static void __attribute__((noinline)) end_kernel(uint8_t val)
{
    // Careful! Adding stuff to this function that uses anything other than
    //  basic registers might cause GCC to restore ra after we changed it
    uint32_t old_ra;
    asm volatile("add a0, %0, zero" : : "r"(val)); // This is a nop, but it prevents GCC from optimizing 'val' out
    asm volatile("csrr %0, mscratch" : "=r"(old_ra));
    asm volatile("add ra, %0, zero" : : "r"(old_ra)); // Clobber ra but don't tell GCC
}

void __attribute__((noreturn)) sys_yield(void)
{
    // Only run the number of programs defined
    if (++cur_prog == NUM_PROGS)
    {
        sys_print_str("\n[kern]: last program run, exiting\n");
        end_kernel(1);
        __builtin_unreachable();
    }

    sys_print_str("\n[kern]: starting program ");
    sys_print_hex(cur_prog + 1);
    sys_print_str("\n\n");

    // Setup the proper ending function
    uint32_t new_ret = (uint32_t) yield;
    asm volatile("add ra, %0, zero" : : "r"(new_ret));

    asm volatile("csrw mepc, %0" : : "r"(progs[cur_prog]));
    uint32_t new_sp = 0x800FFF0;
    asm volatile("add sp, %0, zero" : : "r"(new_sp));
    asm volatile("mret");

    __builtin_unreachable();
}
