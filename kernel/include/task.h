#ifndef TASK_H
#define TASK_H

#include <string.h>
#include "paging.h"

struct task {
	struct cpu_state* cpu_state;
	struct task* next;
	struct virtual_context* context;
};

struct task* init_task(void* entry);
void init_multitasking();
struct cpu_state* schedule(struct cpu_state* cpu);
void switchContext();



#endif 
