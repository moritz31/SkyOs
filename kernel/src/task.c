#include <stdint.h>
#include <string.h>
#include "intr.h"
#include "console.h"
#include "task.h"
#include "memorymanager.h"
#include "paging.h"

static struct task* first_task = NULL;
static struct task* current_task = NULL;

void idle_task() {
	while(1) {

	}
}

void task_a() {
	while(1) {
		kprintf("Hallo\n");
	}
}

void task_b() {
	while(1) {
		kprintf("Welt\n");
	}
}

struct task* init_task(void* entry) {
	
	uint8_t* stack =  (void*)vmem_alloc();
	uint8_t* user_stack = (void*)vmem_alloc();

	//initalize an empty struct 
	struct cpu_state new_state = {
		.eax = 0,
		.ebx = 0,
		.ecx = 0,
		.edx = 0,
		.esi = 0,
		.edi = 0,
		.ebp = 0,

		.eip = (uint32_t) entry,
		.esp = (uint32_t) user_stack + 4096,

		.cs  = 0x18 | 0x03,
		.ss  = 0x20 | 0x03,

		.eflags = 0x200,
	};
	
	struct cpu_state* state = (void*) (stack + 4096 - sizeof(new_state));
	*state = new_state;
	
	struct task* task = (void*) vmem_alloc();
	task->cpu_state = state;
	task->next = first_task;
	first_task = task;

	return task;
}

void init_multitasking() {
	//init_task(task_a);
	//init_task(task_b);
	init_task(idle_task);
}



struct cpu_state* schedule(struct cpu_state* cpu) {
	
	if(current_task != NULL) {
		current_task->cpu_state = cpu;
	}
	
	if(current_task == NULL) {
		current_task = first_task;
	} else {
		current_task = current_task->next;
		if(current_task == NULL) {
			current_task = first_task;
		}
	}
	
	cpu = current_task->cpu_state;
	return cpu;

}
