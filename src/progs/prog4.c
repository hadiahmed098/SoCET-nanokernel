#include "prog4.h"
#include "lib.h"

void USER_PROG(prog4) prog4_main(void)
{
    print_str("[prog4]: Triggering a time interrupt in ~1000 ticks\n");
    uint32_t old_time = read_time(0);
    set_timeout(1000);
    uint32_t time;
    while((time = read_time(0)) < (old_time + 100)); // spin while the time interrupt has not triggered
    print_str("[prog4]: An interrupt trace dump should have been printed\n");
}
