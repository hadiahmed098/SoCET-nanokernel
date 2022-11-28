#include "lib.h"
#include "../lib_common.h"

// These attributes prevent this function from being optimized out
static void __attribute__((noinline)) __attribute__((optimize("O0"))) __attribute__((naked)) __attribute__((section(".userprog.lib")))
make_ecall(int num, void* arg)
{
    asm volatile("ecall");
    asm volatile("ret");
}

void USER_PROG(lib) print_str(const char* buf)
{
    make_ecall(PRINT_STR, (void*)buf);
}

void USER_PROG(lib) print_hex(uint32_t num)
{
    make_ecall(PRINT_HEX, &num);
}

void USER_PROG(lib) yield(void)
{
    make_ecall(YIELD, 0);
}

void USER_PROG(lib) set_timeout(uint16_t interval)
{
    make_ecall(TIME, &interval);
}

uint32_t USER_PROG(lib) read_time(int upper)
{
    uint32_t temp;
    if (upper)
    {
        asm volatile("csrr %0, timeh" : "=r"(temp));
    }
    else
    {
        asm volatile("csrr %0, time" : "=r"(temp));
    }

    return temp;
}

uint32_t USER_PROG(lib) read_cycle(int upper)
{
    uint32_t temp;
    if (upper)
    {
        asm volatile("csrr %0, cycleh" : "=r"(temp));
    }
    else
    {
        asm volatile("csrr %0, cycle" : "=r"(temp));
    }

    return temp;
}
