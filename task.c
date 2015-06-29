#include <stdint.h>
#include "intr.h"
#include "console.h"

const uint8_t stack_a[4096];
const uint8_t stack_b[4096];

int currentTask = -1;
int numberOfTasks = 0;
struct cpu_state* cpu_states[2];

void task_a() {
	while(1) {
		kprintf("A");
	}
}

void task_b() {
	while(1) {
		kprintf("B");
	}
}

struct cpu_state* init_task(uint8_t* stack, void* task) {
	
	//initalize an empty struct 
	struct cpu_state new_state = {
		.eax = 0,
		.ebx = 0,
		.ecx = 0,
		.edx = 0,
		.esi = 0,
		.edi = 0,
		.ebp = 0,

		.eip = (uint32_t) task,

		.cs  = 0x08,

		.eflags = 0x202,
	};
	
	struct cpu_state* state = (void*) (stack + 4096 - sizeof(new_state));
	*state = new_state;

	numberOfTasks++;	

	return state;
}

void init_multitasking() {
	cpu_states[0] = init_task(stack_a,task_a);
	cpu_states[1] = init_task(stack_b,task_b);
}



struct cpu_state* schedule(struct cpu_state* actual_state) {

	if(currentTask >= 0) {
		cpu_states[currentTask] = actual_state;
	}

	currentTask++;
	currentTask %= numberOfTasks;

	return cpu_states[currentTask];

}
