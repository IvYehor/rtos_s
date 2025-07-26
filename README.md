# rtos_s

This is a custom RTOS for ARM Cortex-M MCUs.

# Features

* Multithreading
* Dynamically create/delete tasks with custom heap management

# Description

The base of the project (STM32 HAL) was generated with STM32CUBEMX. On top of that the following files were created:

* app.c/app.h - used for testing the OS
* os.c/os.h - implement the main API functions
* custom_heap.c/custom_heap.h - implement custom heap memory management
* scheduler.c - implements the scheduler

app.c implements a function run_app that is used for testing the OS. The main function then just calls run_app. 

os.c/os.h implements the following OS API:

* StartScheduler - a function to start the OS and ever return
* CreateTask - dynamically create a task
* DeleteTask - dynamically delete a task
* start_critical - enter a critical section (turn off interrupts)
* end_critical - exit a critical section (turn off interrupts)

scheduler.c implements the Sched_handler function that is used as the SysTick interrupt handler.

custom_heap.c/custom_heap.h implement allocate_thread_stack and free_thread_stack used internally by CreateTask and DeleteTask to allocate new memory for stack from the global preallocated heap_memory array.
