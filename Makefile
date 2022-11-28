TOP_NAME=meminit

BINS=$(TOP_NAME).bin
ELFS=$(TOP_NAME).elf

CC=riscv64-unknown-elf-gcc
CCFLAGS=-march=rv32i -mabi=ilp32 -mcmodel=medany -static -nostdlib -O2 -Tlink.ld -Isrc

OBJCPY=riscv64-unknown-elf-objcopy
OBJCPYFLAGS=-O binary

OBJDMP=riscv64-unknown-elf-objdump
OBJDMPFLAGS=-d

STARTUP=startup/crt0.s
SRCS=src/*.c src/progs/*.c

.PHONY: all clean disassemble
.PRECIOUS: $(ELFS)

all: $(BINS)
	cp $(BINS) ../RISCVBusiness/

clean:
	rm dump.txt $(ELFS) $(BINS)

disassemble:
	$(OBJDMP) $(OBJDMPFLAGS) $(ELFS) > dump.txt

%.elf: $(SRCS)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(SRCS) $(STARTUP) -o $(ELFS)

%.bin: %.elf
	$(OBJCPY) $(OBJCPYFLAGS) $(ELFS) $(BINS)
