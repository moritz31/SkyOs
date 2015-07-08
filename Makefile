SRCS = $(shell find -H *.[cS])
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = i686-elf-gcc
LD = i686-elf-ld

ASFLAGS = -m32
CFLAGS = -m32 -Wall -g -fno-stack-protector -nostdinc -Iinclude
LDFLAGS = -melf_i386 -Tlinker.ld

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)

.PHONY: clean
