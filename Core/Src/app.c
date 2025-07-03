/*
 * app.c
 *
 *  Created on: Jun 26, 2025
 *      Author: yehor
 */

#include <stdint.h>
#include <stdlib.h>

#include "stm32f3xx_hal.h"

#include "app.h"


#define STACK_SIZE 256
#define SCHEDULER_PERIOD_MS 10
#define MAX_THREADS 10

struct TCB {
	uint32_t *stack;
	uint32_t *sp;
	uint32_t state;
};

// The word is thread X
// Scheduler variables

uint32_t current_thread;
uint32_t scheduler_tick;
uint32_t num_of_threads_allocated;

// Array of TCBs for the tasks
struct TCB tasks[MAX_TASKS];



void start_critical(void) {
	__disable_irq();
}

void end_critical(void) {
	__enable_irq();
}


// Assigns stack statically because it needs 8-byte alignment
// Does malloc provide 8-byte alignment?
// Uses STACK_SIZE


void setup_stack(uint32_t *stack_pt, uint32_t **thread_sp, void (*thread_func)(void)) {
	uint32_t *thread_stack_top = (uint32_t *)stack_pt + STACK_SIZE;

	*(--thread_stack_top) = 0x01000000; // status register
	*(--thread_stack_top) = (uint32_t)thread_func; // Return address
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
uint32_t CreateTask(void (*thread_func)(void)) {
	start_critical();

	if(num_of_threads_allocated >= MAX_TASKS) {
		// Too many threads allocated
		end_critical();
		return 1;
	}


	// Create the stack
	uint32_t *stack = (uint32_t *)aligned_alloc(sizeof(uint32_t) * STACK_SIZE, 8);

	tasks[num_of_threads_allocated].stack = stack;

	setup_stack(stack, &(tasks[num_of_threads_allocated].sp), thread_func);

	// Set the task state




	num_of_threads_allocated++;

	end_critical();
}


void StartScheduler(void) {

	//setup_stack(thread1_stack, &thread1_sp, thread1_func);
	//setup_stack(thread2_stack, &thread2_sp, thread2_func);

	current_thread = 0;
	scheduler_tick = 0;


	SysTick->LOAD  = (uint32_t)(8000 * SCHEDULER_PERIOD_MS - 1);                         /* set reload register */
	NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
	SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	while(1) {

	}
}













// PA5 thread1
// PB1 thread2

void thread1_func(void) {
	uint32_t led_on = 0;
	uint32_t prev_sched_tick = 0;

	while(1) {
		if (led_on) {
			start_critical();
			GPIOA->BSRR = (0x1 << 5) << 0;
			end_critical();
			led_on = 0;
		}
		else {
			start_critical();
			GPIOA->BSRR = (0x1 << 5) << 16;
			end_critical();
			led_on = 1;
		}

		// 100 * SCHEDULER_PERIOD_MS is 1 s
		uint32_t current_scheduler_tick = 0;
		while(current_scheduler_tick < prev_sched_tick + 100) {
			start_critical();
			current_scheduler_tick = scheduler_tick;
			end_critical();
		}
		prev_sched_tick = scheduler_tick;
	}
}

void thread2_func(void) {
	uint32_t led_on = 0;
	uint32_t prev_sched_tick = 0;

	while(1) {
		if (led_on) {
			start_critical();
			GPIOB->BSRR = (0x1 << 1) << 0;
			end_critical();
			led_on = 0;
		}
		else {
			start_critical();
			GPIOB->BSRR = (0x1 << 1) << 16;
			end_critical();
			led_on = 1;
		}

		// 100 * SCHEDULER_PERIOD_MS is 1 s
		uint32_t current_scheduler_tick = 0;
		while(current_scheduler_tick < prev_sched_tick + 70) {
			start_critical();
			current_scheduler_tick = scheduler_tick;
			end_critical();
		}
		prev_sched_tick = scheduler_tick;
	}
}



//uint32_t thread2_stack[STACK_SIZE] __attribute__((aligned (8)));


void run_app() {


	GPIOA->BSRR = (0x1 << 5) << 0;
	GPIOB->BSRR = (0x1 << 1) << 0;

	CreateTask(thread1_func);
	CreateTask(thread2_func);

	StartScheduler();

	while(1) {

	}


}




__attribute__((naked))
void sched(void) {
	__asm volatile(
			"nop"
	);
}

void pendsv(void) {

}
