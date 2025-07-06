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
#include "os.h"

// The word is thread X


// PA5 thread1
// PB1 thread2

struct TCB *task1_handler;
struct TCB *task2_handler;

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



void run_app() {


	GPIOA->BSRR = (0x1 << 5) << 0;
	GPIOB->BSRR = (0x1 << 1) << 0;

	CreateTask(thread1_func, &task1_handler);
	CreateTask(thread2_func, &task2_handler);

	StartScheduler();

	while(1) {

	}


}



/*void pendsv(void) {

}*/
