.section multiboot
.set MB_PAGE_ALIGN, 1<<0
.set MB_MEMORY_INFO,1<<1  
.set MB_MAGIC, 0x1BADB002
.set MB_FLAGS, MB_PAGE_ALIGN | MB_MEMORY_INFO
.set MB_CHECKSUM, -(MB_MAGIC + MB_FLAGS)
 
// Multiboot header
.align 4
.int    MB_MAGIC
.int    MB_FLAGS
.int    MB_CHECKSUM
 
.section .text
 
// init function 
.extern init

//start 
.global _start
_start:
    //initalize the stack
    mov $kernel_stack, %esp
    // push multiboot info 
    push %eax;
    push %ebx;
    // call the init function
    call init
 
    jmp .
 
//8kb stackspace
.section .bss
.space 8192
kernel_stack:
