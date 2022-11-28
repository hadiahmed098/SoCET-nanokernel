#include "prog3.h"
#include "lib.h"

void USER_PROG(prog3) prog3_main(void)
{
    print_str("[prog3]: I am an evil program!\n");
    print_str("[prog3]: I shall try to read the kernel's memory!\n");
    uint32_t *mtime = (uint32_t*) 0xFFFFFFE0;
    uint32_t val = *mtime;
    print_hex(val);
    print_str("\n[prog3]: Haha, the memory was read!\n");
}