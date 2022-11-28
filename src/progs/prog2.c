#include "prog2.h"
#include "lib.h"

void USER_PROG(prog2) prog2_main(void)
{
    uint32_t time = read_time(0);
    uint32_t cycle = read_cycle(0);
    print_str("[prog2]: It is currently ");
    print_hex(time);
    print_str(" with cycle count ");
    print_hex(cycle);
    print_str("\n");

    print_str("[prog2]: spinning for some time.\n");

    int i; 
    while(i < 1000000) i++;

    time = read_time(0);
    cycle = read_cycle(0);
    print_str("[prog2]: It is now ");
    print_hex(time);
    print_str(" with cycle count ");
    print_hex(cycle);
    print_str("\n");
}