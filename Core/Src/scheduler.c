/**
 * @file scheduler.c
 *
 *  Created on: Jul 3, 2025
 *      Author: yehor
 */

#include <stdlib.h>

#include "os.h"
#include "custom_heap.h"


// Used -fomit-frame-pointer
// Clobbering sp is depricated
// "This is because the compiler requires the value of the stack pointer to be the same after an asm statement as it was on entry to the statement. "
// Is CPSID I really needed?
// Imagine there are no interrupts except for the systick interrupt
// r12?

// sp depricated problem
// lr not pushed problem

// Do not use stack here at all (not really). Only registers and global variables


/**
 * @brief The scheduler function
 *
 * For proper execution of the OS, the Systick interrupt should be assigned to this function
 * */
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


	// Save sp

	if(current_thread != NO_TASK_RUNNNING) {
		static uint32_t **old_sp;
		old_sp = &tasks[current_thread].sp;
		__asm volatile(
			"MOV %0, sp\n"
			: "=r" (*old_sp)
			:
			:
		);
	}

	// Switch to the scheduler stack
	__asm volatile(
		"MOV sp, %0 \n"
		"PUSH {lr} \n" // ?????
		:
		: "r" (scheduler_sp)
		: "sp"
	);

	// Increment scheduler tick
	++scheduler_tick;

	// Choose the next thread here
	// There must be at least one thread.

	//Does not push lr when calling free_thread_stack

	static uint32_t thread_index;
	for(thread_index = 0; thread_index < MAX_THREADS; ++thread_index) {
		if(tasks[thread_index].pending_delete) {
			tasks[thread_index].pending_delete = 0;
			tasks[thread_index].allocated = 0;

			free_thread_stack(tasks[thread_index].stack);
			//free(tasks[thread_index].stack);
		}
	}

	static uint32_t *new_sp;

	static uint32_t relative_thread_index;

	new_sp = NULL;
	if(current_thread == NO_TASK_RUNNNING) {
		current_thread = MAX_THREADS-1;
		// Loop after the last one (starting from 0)
	}
	// Loop around the threads starting from the current one (including the current one at the end)
	for(relative_thread_index = 0; relative_thread_index < MAX_THREADS; ++relative_thread_index) {
		thread_index = (current_thread + relative_thread_index + 1) % MAX_THREADS;
		if(tasks[thread_index].allocated && tasks[thread_index].sleep_for_ticks == 0) {
			new_sp = tasks[thread_index].sp;
			current_thread = thread_index;
			break;
		}
		--tasks[thread_index].sleep_for_ticks;
	}

	if(new_sp == NULL) {
		// No threads are allocated
		while(1);
	}
	/*if(current_thread == NO_TASK_RUNNNING) {
		// This is the first execution of the scheduler function
		new_sp = tasks[0].sp;
		current_thread = 0;
	}
	else {
		uint32_t next_thread = (current_thread + 1) % num_of_threads_allocated;
		new_sp = tasks[next_thread].sp;

		current_thread = next_thread;
	}*/


	// Set sp
	__asm volatile(
		"POP {LR} \n"
		"MOV sp, %0 \n"
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

