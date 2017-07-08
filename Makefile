SRCDIR   = kernel/src
INCLUDEDIR = kernel/include
OBJDIR    = obj
BINDIR = bin

CFILES := $(foreach dir, $(SRCDIR)/, $(notdir $(wildcard $(SRCDIR)/*.c)))
ASMFILES := $(foreach dir, $(SRCDIR)/, $(notdir $(wildcard $(SRCDIR)/*.S)))
OBJS := $(addprefix $(OBJDIR)/, $(CFILES:.c=.o) $(ASMFILES:.S=.o))

CC = i686-elf-gcc
LD = i686-elf-ld

ASFLAGS = -m32
CFLAGS =  -g -m32 -Wall -g -fno-stack-protector -nostdinc -I$(INCLUDEDIR)
LDFLAGS = -melf_i386 -Tlinker.ld

TARGET = kernel.elf

$(BINDIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(LD) $(LDFLAGS) -o $@ $^

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(info ======  $< ---> $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.S
	@mkdir -p $(OBJDIR)
	$(info ======  $< ---> $@)
	@$(CC) $(ASFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -r $(OBJDIR)
	@rm -r $(BINDIR)

.PHONY: debug
debug: $(BINDIR)/$(TARGET)
	qemu-system-i386 -s -S -m 256 -kernel $<