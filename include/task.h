#ifndef TASK_H
#define TASK_H

struct cpu_state* init_task(uint8_t* stack, void* task);
void init_multitasking();
struct cpu_state* schedule(struct cpu_state* actualy_state);



#endif 
