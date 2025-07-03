/*
 * os.h
 *
 *  Created on: Jul 3, 2025
 *      Author: yehor
 */

#ifndef SRC_OS_H_
#define SRC_OS_H_

#include <stdint.h>

#define STACK_SIZE 256
#define SCHEDULER_PERIOD_MS 10
#define MAX_THREADS 10
#define NO_TASK_RUNNNING MAX_THREADS

struct TCB {
	uint32_t *stack;
	uint32_t *sp;
	uint32_t state;
};

extern uint32_t current_thread;
extern uint32_t scheduler_tick;
extern uint32_t num_of_threads_allocated;

// Array of TCBs for the tasks
extern struct TCB tasks[MAX_THREADS];


void start_critical(void);

void end_critical(void);





#endif /* SRC_OS_H_ */
