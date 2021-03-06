.macro intr_stub nr
.global intr_stub_\nr
intr_stub_\nr:
    pushl $0
    pushl $\nr
    jmp intr_common_handler
.endm

.macro intr_stub_error_code nr
.global intr_stub_\nr
intr_stub_\nr:
    pushl $\nr
    jmp intr_common_handler
.endm

// Exceptions
intr_stub 0
intr_stub 1
intr_stub 2
intr_stub 3
intr_stub 4
intr_stub 5
intr_stub 6
intr_stub 7
intr_stub_error_code 8
intr_stub 9
intr_stub_error_code 10
intr_stub_error_code 11
intr_stub_error_code 12
intr_stub_error_code 13
intr_stub_error_code 14
intr_stub 15
intr_stub 16
intr_stub_error_code 17
intr_stub 18

//IRQ's
intr_stub 32
intr_stub 33
intr_stub 34
intr_stub 35
intr_stub 36

//Syscalls
intr_stub 48

.extern handle_interrupt
intr_common_handler:
    // CPU-Zustand sichern
    push %ebp
    push %edi
    push %esi
    push %edx
    push %ecx
    push %ebx
    push %eax
 
    //load kernel datasegment
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es

    // Handler aufrufen
    push %esp
    call handle_interrupt
    mov %eax, %esp
    //load user datasegment
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es
 
    // CPU-Zustand wiederherstellen
    pop %eax
    pop %ebx
    pop %ecx
    pop %edx
    pop %esi
    pop %edi
    pop %ebp
 
    // Fehlercode und Interruptnummer vom Stack nehmen
    add $8, %esp
 
    // Ruecksprung zum unterbrochenen Code
    iret
	
