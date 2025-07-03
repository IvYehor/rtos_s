/*
 * scheduler.c
 *
 *  Created on: Jul 3, 2025
 *      Author: yehor
 */

#include <stdlib.h>

#include "os.h"



// Used -fomit-frame-pointer
// Clobbering sp is depricated
// "This is because the compiler requires the value of the stack pointer to be the same after an asm statement as it was on entry to the statement. "
// Is CPSID I really needed?
// Imagine there are no interrupts except for the systick interrupt
// r12?

__attribute__((naked))
void Sched_handler(void) {
	// Save context
	__asm volatile(
			"CPSID I\n"
			:
			:
			: "memory", "cc"
	);

	__asm volatile(
				"PUSH {r4-r11}"
				:
				:
				: "memory", "cc"
		);
	// Do not use stack here at all. Only registers and global variables

	++scheduler_tick;

	// Choose the next thread here
	// There must be at least one thread

	static uint32_t *new_sp;
	static uint32_t **old_sp;
	if(current_thread == NO_TASK_RUNNNING) {
		// This is the first execution of the scheduler function
		new_sp = tasks[0].sp;
		old_sp = NULL;
		current_thread = 0;
	}
	else {
		uint32_t next_thread = (current_thread + 1) % MAX_THREADS;
		new_sp = tasks[next_thread].sp;
		old_sp = &tasks[current_thread].sp;
		current_thread = next_thread;
	}

	// Save sp
	if(old_sp != NULL) {
		__asm volatile(
			"MOV %0, sp\n"
			: "=r" (*old_sp)
			:
			:
		);
	}

	// Set sp
	__asm volatile(
		"MOV sp, %0"
		:
		: "r" (new_sp)
		: "sp"
	);


	//Restore context
	__asm volatile(
			"POP {r4-r11}\n"
			"CPSIE I\n"
			:
			:
			: "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "memory", "cc"
		);
	__asm volatile(
			"BX LR"
			:
			:
			: "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "memory", "cc"
	);




}

