/*
 * custom_heap.h
 *
 *  Created on: Jul 10, 2025
 *      Author: yehor
 */

#ifndef INC_CUSTOM_HEAP_H_
#define INC_CUSTOM_HEAP_H_

#include "os.h"

uint32_t *allocate_thread_stack(void);

enum ErrorCode free_thread_stack(uint32_t *thread_stack);

#endif /* INC_CUSTOM_HEAP_H_ */
