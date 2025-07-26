/**
 * @file custom_heap.h
 * @brief The header file with the declarations of functions for the custom heap management.
 *
 */

#ifndef INC_CUSTOM_HEAP_H_
#define INC_CUSTOM_HEAP_H_

#include "os.h"

/**
 * @brief A function that allocates the memory for a new thread stack
 * @return A pointer to the bottom of stack
 *
 * The amount of memory to allocate for a stack is predefined by STACK_SIZE.
 * */
uint32_t *allocate_thread_stack(void);

/**
 * @brief A function to free the previously allocated stack
 * @param thread_stack The pointer to the bottom of the stack
 * @return An error code
 *
 * The "free" operation is not successful if NULL is passed or is the space is already free
 * */
enum ErrorCode free_thread_stack(uint32_t *thread_stack);

#endif /* INC_CUSTOM_HEAP_H_ */
