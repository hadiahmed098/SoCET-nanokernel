#ifndef __LIB_INTERNAL_H__
#define __LIB_INTERNAL_H__

#include <stdint.h>

void handler(void);

void sys_print_str(char *);

void sys_print_hex(uint32_t);

void sys_yield(void);

#endif