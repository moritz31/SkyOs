#include <stdint.h>
#include "console.h"
#include "intr.h"
#include "serial.h"
#include "task.h"


extern void intr_stub_0(void);
extern void intr_stub_1(void);
extern void intr_stub_2(void);
extern void intr_stub_3(void);
extern void intr_stub_4(void);
extern void intr_stub_5(void);
extern void intr_stub_6(void);
extern void intr_stub_7(void);
extern void intr_stub_8(void);
extern void intr_stub_9(void);
extern void intr_stub_10(void);
extern void intr_stub_11(void);
extern void intr_stub_12(void);
extern void intr_stub_13(void);
extern void intr_stub_14(void);
extern void intr_stub_15(void);
extern void intr_stub_16(void);
extern void intr_stub_17(void);
extern void intr_stub_18(void);

extern void intr_stub_32(void);//PIT
extern void intr_stub_33(void);//Keyboard
extern void intr_stub_34(void);//Cascade
extern void intr_stub_35(void);//COM2
extern void intr_stub_36(void);//COM1

extern void intr_stub_48(void);

uint32_t tss[32] = {0,0 ,0x10};

/************************
******* GDT *************
************************/

#define GDT_ENTRIES 6

#define GDT_FLAG_DATASEG  0x02
#define GDT_FLAG_CODESEG  0x0a
#define GDT_FLAG_TSS      0x09

#define GDT_FLAG_RING0    0x00
#define GDT_FLAG_SEGMENT  0x10
#define GDT_FLAG_RING3    0x60
#define GDT_FLAG_PRESENT  0x80

#define GDT_FLAG_4K       0x800
#define GDT_FLAG_32_BIT   0x400

static uint64_t gdt[GDT_ENTRIES];

/*
	This function set an right gdt entry based on the input
*/
static void gdt_set_entry(int i,unsigned int base, unsigned int limit,int flags) {
	gdt[i] = limit & 0xffffLL;
        gdt[i] |= (base & 0xffffffLL) << 16;
        gdt[i] |= (flags & 0xffLL) << 40;
        gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
        gdt[i] |= ((flags >> 8 )& 0xffLL) << 52;
        gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}

/*
	Initalize the GDT
*/	
void init_gdt(void) {
	//gdt pointer struct
	struct {
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) gdtp = {
		.limit = GDT_ENTRIES * 8 - 1,
		.pointer = gdt,
	};
	
	//set the gdt entries 
	gdt_set_entry(0,0,0,0);
	gdt_set_entry(1,0,0xfffff,GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT |
		GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
	gdt_set_entry(2,0,0xfffff,GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT |
		GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
	gdt_set_entry(3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT |
                GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
       	gdt_set_entry(4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT |
                GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	gdt_set_entry(5, (uint32_t) tss, sizeof(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | 		GDT_FLAG_RING3);
	
	//set gdt pointer
	asm volatile("lgdt %0" : : "m" (gdtp));
	//load gdt
	asm volatile(
           "mov $0x10, %ax;"
           "mov %ax, %ds;"
           "mov %ax, %es;"
           "mov %ax, %ss;"
           "ljmp $0x8, $.1;"
           ".1:"
       );

	//reload taskregister
	asm volatile("ltr %%ax" : : "a" (5 << 3));
}

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a" (val), "Nd" (port));
}

/************************
******* PIC *************
************************/

/*
	Initalize the PIC so that the Interrupts start at 0x20
*/
void init_pic(void) {
	
	//PIC 1 init
	outb(0x20,0x11);
	outb(0x21,0x20); //Interruptnumber IRQ 0
	outb(0x21,0x04); //Slave on IRQ 2
	outb(0x21,0x01); //ICW 4
	//PIC 2 init
	outb(0xa0,0x11);
	outb(0xa1,0x28); //Interruptnumber IRQ 8
	outb(0xa1,0x02); //Slave on IRQ 2
	outb(0xa1,0x01); //ICW 4
	//enable interrups
	outb(0x20,0x0);
	outb(0xa0,0x0);
}

/************************
******* PIT *************
************************/

static void init_pit(int freq) {	
	int counter = 1193182 / freq;
	outb(0x43,0x34);
	outb(0x40,counter & 0xff);
	outb(0x40,counter >> 8);
}

/************************
******* IDT *************
************************/

#define IDT_ENTRIES 256

#define IDT_FLAG_INTERRUPT_GATE 0xe
#define IDT_FLAG_PRESENT 	0x80
#define IDT_FLAG_RING0		0x00
#define IDT_FLAG_RING3		0x60

static uint64_t idt[IDT_ENTRIES];

/*
	This function set an right idt entry based on the input
*/
static void idt_set_entry(int i, void (*fn)(), unsigned int selector, int flags) {

	unsigned long int handler = (unsigned long int) fn;
	idt[i] = handler & 0xffffLL;
	idt[i] |= (selector & 0xffffLL) << 16;
	idt[i] |= (flags & 0xffLL) << 40;
	idt[i] |= ((handler >> 16) & 0xffffLL) << 48;

}

/*
	Initalize idt and activate the interrupts
*/
void init_idt(void) {
	
	struct {
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) idtp = {
		.limit = IDT_ENTRIES * 8 - 1,
		.pointer = idt,
	};

	//init the pic to set the right irq numbers
	init_pic();

	//init_pit(1);

	//Exception-Handler
	idt_set_entry(0,intr_stub_0, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(1,intr_stub_1, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(2,intr_stub_2, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(3,intr_stub_3, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(4,intr_stub_4, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(5,intr_stub_5, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(6,intr_stub_6, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(7,intr_stub_7, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(8,intr_stub_8, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(9,intr_stub_9, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(10,intr_stub_10, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(11,intr_stub_11, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(12,intr_stub_12, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(13,intr_stub_13, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(14,intr_stub_14, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(15,intr_stub_15, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(16,intr_stub_16, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(17,intr_stub_17, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(18,intr_stub_18, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	
	//IRQ-Handler
	idt_set_entry(32,intr_stub_32, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(33,intr_stub_33, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(34,intr_stub_34, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(35,intr_stub_35, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(36,intr_stub_35, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

	//Syscalls
	idt_set_entry(48,intr_stub_48, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING3 | IDT_FLAG_PRESENT);


	//load the idt
	asm volatile("lidt %0" : : "m" (idtp));

    	asm volatile("sti");

}


/************************
******* Interrupts ******
************************/

struct cpu_state* handle_interrupt(struct cpu_state* cpu) {
	//TODO 
	//create a new cpu struct and save the cpu to them
	struct cpu_state* new_cpu = cpu;

	if(cpu->intr <= 0x1f) {
		kprintf("Exception %d, kernel stopped!\n", cpu->intr);

		while(1) {
			asm volatile("cli; hlt");
		}
	} else if(cpu->intr >= 0x20 && cpu->intr <= 0x2f) {

		//kprintf("Interrupt %d", cpu->intr);

		if(cpu->intr == 0x20) {
			new_cpu = schedule(cpu);
			tss[1] = (uint32_t) (new_cpu + 1);
		}

		//send end of interrupts
		if (cpu->intr >= 0x28) {
			//PIC 2 EOI
			outb(0xA0,0x20);
		}
		//PIC 1 EOI
		outb(0x20, 0x20);
			
	} else {
		kprintf("Unkown interrupt!");
		while(1) {
			asm volatile("cli; hlt");
		}
	}
	
	//return the new cpu, which can be the old one inf not scheduled
	return new_cpu;

}

