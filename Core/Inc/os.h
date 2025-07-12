/*
 * os.h
 *
 *  Created on: Jul 3, 2025
 *      Author: yehor
 */

#ifndef SRC_OS_H_
#define SRC_OS_H_

#include <stdint.h>

// stack size in words
// Should be divisible by 2?
#define STACK_SIZE 256

#define SCHEDULER_STACK_SIZE 256

#define SCHEDULER_PERIOD_MS 10
#define MAX_THREADS 5
#define NO_TASK_RUNNNING MAX_THREADS

// heap size in words
#define HEAP_SIZE 256*5


//enum TaskState { NOT_ALLOCATED, READY };

struct TCB {
	uint32_t *stack;
	uint32_t *sp;
	uint32_t allocated;
	uint32_t pending_delete;
};

enum ErrorCode { ERROR_OK, ERROR_FAIL };

extern uint32_t *scheduler_sp;
extern uint32_t current_thread;
extern uint32_t scheduler_tick;
//extern uint32_t num_of_threads_allocated;
extern uint32_t os_running;

// Array of TCBs for the tasks
extern struct TCB tasks[MAX_THREADS];


void start_critical(void);

void end_critical(void);


enum ErrorCode CreateTask(void (*thread_func)(void), struct TCB **returned_tcb);
void DeleteTask(struct TCB *task);

void StartScheduler(void);


// Pass arguments to threads
// Mutexes
// Delays (accurate delayes?)
// Task wrappers????
// Priorities?


#endif /* SRC_OS_H_ */
