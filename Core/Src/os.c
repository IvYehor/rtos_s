/**
 * @file os.c
 *
 *  Created on: Jul 3, 2025
 *      Author: yehor
 */

#include "os.h"
#include "cmsis_gcc.h"
#include "stm32f3xx_hal.h"
#include "custom_heap.h"
#include <stdlib.h>

uint32_t current_thread;
uint32_t scheduler_tick;
uint32_t *scheduler_sp;
uint32_t os_running = 0;

// Array of TCBs for the tasks
struct TCB tasks[MAX_THREADS];

uint32_t scheduler_stack_array[SCHEDULER_STACK_SIZE] __attribute__ ((aligned(8)));


/*
 *  There are threads that change some variables while they run
 *  And there is a scheduler that tries to deal with what the threads told it to do
 *
 *
 *
 *
 * */

// Call scheduler immediately when wants to delay or delete?
// Set some magic values to check if stacks got out of range

void start_critical(void) {
	__disable_irq();
}

void end_critical(void) {
	__enable_irq();
}

// Assigns stack statically because it needs 8-byte alignment
// Does malloc provide 8-byte alignment?
// Uses STACK_SIZE
// Error code convention


void setup_stack(uint32_t *stack_pt, uint32_t **thread_sp, void (*thread_func)(void)) {
	uint32_t *thread_stack_top = (uint32_t *)stack_pt + STACK_SIZE;


	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU
	*(--thread_stack_top) = 0x0; // FPU

	*(--thread_stack_top) = 0x01000000; // status register
	// Thumb state: & ~0x1
	*(--thread_stack_top) = ((uint32_t)thread_func) | 0x1; // Return address
	*(--thread_stack_top) = 0xFFFFFFFD; // LR
	*(--thread_stack_top) = 0x12121212; // R12
	*(--thread_stack_top) = 0x03030303; // R3
	*(--thread_stack_top) = 0x02020202; // R2
	*(--thread_stack_top) = 0x01010101; // R1
	*(--thread_stack_top) = (uint32_t)NULL; // R0

	*(--thread_stack_top) = 0x11111111; // R11
	*(--thread_stack_top) = 0x10101010; // R10
	*(--thread_stack_top) = 0x09090909; // R9
	*(--thread_stack_top) = 0x08080808; // R8
	*(--thread_stack_top) = 0x07070707; // R7
	*(--thread_stack_top) = 0x06060606; // R6
	*(--thread_stack_top) = 0x05050505; // R5
	*(--thread_stack_top) = 0x04040404; // R4

	*thread_sp = thread_stack_top;
}

// Returns >0 if error occured
enum ErrorCode CreateTask(void (*thread_func)(void), struct TCB **returned_tcb) {
	start_critical();

	uint32_t new_thread_index = MAX_THREADS;
	for(uint32_t i = 0; i < MAX_THREADS; ++i) {
		if(!tasks[i].allocated) {
			new_thread_index = i;
			break;
		}
	}

	if(new_thread_index == MAX_THREADS) {
		// Too many threads allocated
		end_critical();
		return ERROR_FAIL;
	}


	// Create the stack
	uint32_t *stack = allocate_thread_stack();
	if(stack == NULL) {
		// Could not allocate stack
		return ERROR_FAIL;
	}

	tasks[new_thread_index].stack = stack;

	setup_stack(stack, &(tasks[new_thread_index].sp), thread_func);

	//tasks[new_thread_index].state = READY;
	tasks[new_thread_index].allocated = 1;
	tasks[new_thread_index].pending_delete = 0;


	(*returned_tcb) = &tasks[new_thread_index];

	end_critical();


	return ERROR_OK;
}

void DeleteTask(struct TCB *task) {
	start_critical();

	task->pending_delete = 1;

	end_critical();

	// Out of critical
	// Change to SVC
	if(task == &tasks[current_thread]) {
		while(1);
	}
}

void default_thread_func(void) {
	while(1) {

	}
}

void InitScheduler(void) {
	for(size_t i = 0; i < MAX_THREADS; ++i) {
		tasks[i].allocated = 0;
	}
}

void StartScheduler(void) {

	/*if (num_of_threads_allocated == 0) {
		// No threads were created
		while(1);
	}*/

	uint32_t *scheduler_stack = scheduler_stack_array;

	uint32_t *scheduler_stack_top = (uint32_t *)(scheduler_stack + SCHEDULER_STACK_SIZE);
	scheduler_sp = --scheduler_stack_top;

	current_thread = NO_TASK_RUNNNING;
	scheduler_tick = 0;
	os_running = 1;

	//CreateTask(default_thread_func);

	SysTick->LOAD  = (uint32_t)(8000 * SCHEDULER_PERIOD_MS - 1);                         /* set reload register */
	NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
	SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	while(1) {

	}
}


