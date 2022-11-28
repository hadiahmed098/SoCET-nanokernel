#include "prog1.h"
#include "lib.h"

void USER_PROG(prog1) prog1_main(void)
{
    print_str("[prog1]: ");
    print_hex(0xDEADBEEF);
    print_str("\n");
}