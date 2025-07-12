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
#include "custom_heap.h"

// The word is thread X


// PA5 thread1
// PB1 thread2

struct TCB *creator_handler;
struct TCB *created_handler3;

uint32_t job_finished = 1;

void bad_delay(uint32_t amount_of_ticks) {
	start_critical();
	uint32_t start_scheduler_tick = scheduler_tick;
	end_critical();
	uint32_t current_scheduler_tick = 0;
	while(current_scheduler_tick < start_scheduler_tick + amount_of_ticks) {
		start_critical();
		current_scheduler_tick = scheduler_tick;
		end_critical();
	}
}

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

void created_task(void) {

	GPIOB->BSRR = (0x1 << 1) << 0;

	bad_delay(350);

	start_critical();
	job_finished = 1;
	end_critical();

	DeleteTask(created_handler3);

	while(1) {}
}

void creator_func(void) {
	struct TCB *created_handler1;
	struct TCB *created_handler2;

	CreateTask(thread1_func, &created_handler1);
	//CreateTask(thread2_func, &created_handler2);

	while(1) {
		start_critical();
		uint32_t job_finished_temp = job_finished;
		end_critical();
		if(job_finished_temp) {
			GPIOB->BSRR = (0x1 << 1) << 16;
			bad_delay(100);
			job_finished = 0;
			CreateTask(created_task, &created_handler3);
		}
		//DeleteTask(created_handler);
	}
}


void run_app() {


	GPIOA->BSRR = (0x1 << 5) << 0;
	GPIOB->BSRR = (0x1 << 1) << 0;


	CreateTask(creator_func, &creator_handler);

	StartScheduler();


	while(1) {

	}


}





/*void pendsv(void) {

}*/
