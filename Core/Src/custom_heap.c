/**
 * @file custom_heap.c
 *
 *  Created on: Jul 10, 2025
 *      Author: yehor
 */

#include <stdint.h>
#include <stdlib.h>

#include "custom_heap.h"
#include "os.h"

// HEAP_SIZE
uint32_t heap_memory[HEAP_SIZE] __attribute__ ((aligned(8)));
// Just create the memory for threads and pass the pointers to the beginning

// Return indexes along with stack addresses?

// HEAP_SIZE / STACK_SIZE rounded towards 0 (should generally be divisible)
#define MAX_NUM_OF_STACKS (HEAP_SIZE / STACK_SIZE)
// Use bitfields later
uint8_t stack_allocated[MAX_NUM_OF_STACKS];

// The required behaviour:
// uint32_t *stack = (uint32_t *)aligned_alloc(sizeof(uint32_t) * STACK_SIZE, 8);


//Should be aligned to 8
//can return NULL
uint32_t *allocate_thread_stack(void) {

	//get nth address
	// heap_memory + i * STACK_SIZE
	//get index of address
	// (address - heap_memory) / STACK_SIZE
	for(int i = 0; i < MAX_NUM_OF_STACKS; ++i) {
		if(stack_allocated[i] == 0) {
			// Allocate
			stack_allocated[i] = 1;
			// Return the pointer
			return &heap_memory[i*STACK_SIZE];
		}
	}

	return NULL;
}

enum ErrorCode free_thread_stack(uint32_t *thread_stack) {
	if(thread_stack == NULL) {
		return ERROR_FAIL;
	}

	ptrdiff_t stack_i = (thread_stack - (uint32_t *)heap_memory) / (ptrdiff_t) STACK_SIZE;
	if(stack_allocated[stack_i] == 1) {
		stack_allocated[stack_i] = 0;
		return ERROR_OK;
	}
	else {
		return ERROR_FAIL;
	}
	// Can be nullptr
	// Can be some arbitrary value

	// Should be an address of memory in heap array
	// Should 8 aligned (divisible by 64)

	return ERROR_FAIL;
}


