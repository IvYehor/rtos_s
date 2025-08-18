/**
 * @file os.h
 * @brief The header file with the main OS API declarations
 *
 */

#ifndef SRC_OS_H_
#define SRC_OS_H_

#include <stdint.h>


/// Stack size in words
#define STACK_SIZE 256
// Should be divisible by 2?

/// Stack size for the scheduler in words
#define SCHEDULER_STACK_SIZE 256

#define DEFAULT_TASK_STACK_SIZE 128

#define SCHEDULER_PERIOD_MS 10
#define MAX_THREADS 5

/*
 * Before the fist thread executes, current_thread is equal to NO_TASK_RUNNNING to indicate that OS has just started.
 * During the execution, current_thread is between 0 and MAX_THREADS to indicate the index of the thread that currently executes.
 * */
#define NO_TASK_RUNNNING MAX_THREADS

/// heap size in words preallocated on stack
#define HEAP_SIZE 256*5


struct TCB {
	uint32_t *stack;
	uint32_t *sp;
	uint32_t allocated;
	uint32_t pending_delete;
	uint32_t sleep_for_ticks;
	uint32_t is_default;
	uint32_t index;
};

enum ErrorCode { ERROR_OK, ERROR_FAIL };



extern uint32_t *scheduler_sp;
extern uint32_t current_thread;
extern uint32_t scheduler_tick;
extern uint32_t os_running;

// Array of TCBs for the tasks
extern struct TCB tasks[MAX_THREADS];

// Default thread

extern struct TCB *default_task_tcb;
extern uint32_t default_thread_index;


/**
 * @brief A function to call when entering a critical section
 *
 * The function turns off the interrupts (especially the SysTick interrupt)
 * */
void start_critical(void);


/**
 * @brief A function to call when exiting a critical section
 *
 * The function turns on the interrupts (especially the SysTick interrupt)
 * */
void end_critical(void);


/**
 * @brief Create a new thread
 * @param thread_func A pointer to the thread function
 * @param returned_tcb A pointer a variable where to store the pointer to newly created thread
 * @return An error code
 *
 * Can be called within a thread or before calling StartScheduler. The thread creation is not successful if reached a maximum number of threads or there isn't enough space in heap to allocate a new stack.
 * */
enum ErrorCode CreateTask(void (*thread_func)(void), struct TCB **returned_tcb);


/**
 * @brief A function to delete the thread
 * @param task A pointer to the TCB of the thread to be deleted
 *
 * Does not delete the thread immediately, instead marks the thread to be deleted by the scheduler.
 * Can also be called within the thread that is to be deleted. In that case it just enters a while loop until the SysTick interrupt occurs and it is deleted by the scheduler.
 *
 * */
void DeleteTask(struct TCB *task);


/**
 * @brief The function starts the OS and never returns.
 *
 * */

void StartScheduler(void);

void Delay(uint32_t scheduler_ticks);

// Need to separate private and public declarations
//enum TaskState { NOT_ALLOCATED, READY };

#endif /* SRC_OS_H_ */
