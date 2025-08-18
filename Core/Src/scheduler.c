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

#define atomic_on() \
	__asm volatile( \
			"CPSID I\n" \
			: \
			: \
			: "memory", "cc" \
	)

#define atomic_off() \
	__asm volatile( \
			"CPSIE I\n" \
			: \
			: \
			: "memory", "cc" \
	)


#define save_r4r11() \
	__asm volatile( \
				"PUSH {r4-r11}" \
				: \
				: \
				: "memory", "cc" \
		)

#define restore_r4r11() \
	__asm volatile( \
				"POP {r4-r11}\n" \
				: \
				: \
				: "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "memory", "cc" \
		)


#define save_sp(old_sp) \
	__asm volatile( \
			"MOV %0, sp\n" \
			: "=r" (*old_sp) \
			: \
			: \
		)

// ?????
#define save_lr() \
	__asm volatile( \
				"PUSH {lr} \n" \
				: \
				: \
				: "sp" \
			)

#define restore_lr() \
	__asm volatile( \
				"POP {LR} \n" \
				: \
				: \
				: "sp" \
			)


#define switch_to_sp(new_sp) \
	__asm volatile( \
			"MOV sp, %0 \n" \
			: \
			: "r" (new_sp) \
			: "sp" \
		)

#define exit_scheduler() \
	__asm volatile( \
			"BX LR" \
			: \
			: \
			: "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "memory", "cc" \
		)

__attribute__((naked))
void Sched_handler(void) {
	atomic_on();
	save_r4r11();

	static uint32_t *old_sp;
	save_sp(old_sp);

	switch_to_sp(scheduler_sp);
	// Save lr to be able to call functions
	//Does not push lr when calling free_thread_stack
	save_lr();

	// Scheduler logic starts here ...
	// Choose the next thread here
	// There must be at least one thread.

	// Interrupt from the beginning
	// Interrupt from default thread
	// Interrupt from a regular thread

	if (current_thread == DEFAULT_TASK_RUNNNING) {
		default_thread_sp = old_sp;
	}
	else if (current_thread != NO_TASK_RUNNNING) {
		tasks[current_thread].sp = old_sp;
	}

	// Increment scheduler tick
	++scheduler_tick;

	// Delete pending threads
	static uint32_t thread_index;
	for(thread_index = 0; thread_index < MAX_THREADS; ++thread_index) {
		if(tasks[thread_index].pending_delete) {
			tasks[thread_index].pending_delete = 0;
			tasks[thread_index].allocated = 0;

			free_thread_stack(tasks[thread_index].stack);
			//free(tasks[thread_index].stack);
		}
	}


	// Choose value for new_sp
	static uint32_t *new_sp;
	new_sp = NULL;

	static uint32_t relative_thread_index;

	if(current_thread == NO_TASK_RUNNNING || current_thread == DEFAULT_TASK_RUNNNING) {
		current_thread = MAX_THREADS-1;
		// Loop after the last one (starting from 0)
	}
	// Loop around the threads starting from the current one (including the current one at the end)
	for(relative_thread_index = 0; relative_thread_index < MAX_THREADS; ++relative_thread_index) {
		thread_index = (current_thread + relative_thread_index + 1) % MAX_THREADS;
		if(tasks[thread_index].allocated) {
			new_sp = tasks[thread_index].sp;
			current_thread = thread_index;
			break;
		}
	}

	if(new_sp == NULL) {
		// No threads are allocated
		new_sp = default_thread_sp;
	}

	// Set sp
	restore_lr();
	switch_to_sp(new_sp);

	//Restore context
	restore_r4r11();
	atomic_off();

	exit_scheduler();

}

__attribute__((naked))
void Abstract_Sched_handler(void) {
	atomic_on();
	save_r4r11();

	static uint32_t *old_sp;
	save_sp(old_sp);

	switch_to_sp(scheduler_sp);
	save_lr();

	static uint32_t *new_sp;

	// ...
	// Choose the next stack pointer
	// ... = old_sp
	// new_sp = ...

	restore_lr();
	switch_to_sp(new_sp);

	restore_r4r11();
	atomic_off();

	exit_scheduler();

}

