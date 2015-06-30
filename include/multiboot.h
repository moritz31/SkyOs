#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

typedef struct multiboot_info {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t syms[4];
	uint32_t mmap_length;
	uint32_t mmap_addr;
}__attribute__((packed)) multiboot_info_t;

typedef struct memory_map {
       uint32_t size;
       uint32_t base_addr_low;
       uint32_t base_addr_high;
       uint32_t length_low;
       uint32_t length_high;
       uint32_t type;
}__attribute__((packed)) memory_map_t;

#endif
