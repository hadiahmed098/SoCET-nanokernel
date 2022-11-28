#ifndef __LIB_H__
#define __LIB_H__

#include <stdint.h>

// Prints a string to the console
void print_str(const char*);

// Prints a hex value to the console
void print_hex(uint32_t);

// Terminates the user program
void yield(void);

// Read the 'time' or 'timeh' CSR
uint32_t read_time(int);

// Read the 'cycle' and 'cycleh' CSR
uint32_t read_cycle(int);

// Add this macro to every user program function to add the proper memory protection values
#define USER_PROG(X) __attribute__((noinline, section(".userprog." #X), optimize("-fno-optimize-sibling-calls")))

#endif