/*
 * scheduler.s
 *
 *  Created on: Jun 27, 2025
 *      Author: yehor
 */

/*
.syntax unified
.cpu cortex-m4
.thumb

.global Sched_handler

.extern thread1_sp
.extern thread2_sp
.extern current_thread
.extern scheduler_tick

.section .text
.type	Sched_handler, %function
// Does not compy with AAPCS
Sched_handler:
	CPSID I

	PUSH {r4-r11}

	// Increment scheduler_tick
	LDR r0, =scheduler_tick
	LDR r1, [r0]
	ADD r1, r1, #1
	STR r1, [r0]

	// Decide which thread to run next
	LDR r0, =current_thread
	LDR r0, [r0]
	CMP r0, #0
	// If 0, no thread is running
	BEQ start_thread1
	CMP r0, #1
	// If 1, thread1 is running
	BEQ switch_to_thread2
	// If 2, thread2 is running
	B switch_to_thread1

start_thread1:
	LDR r0, =thread1_sp
	LDR r0, [r0]
	MOV sp, r0

	// Set current_thread to 1
	LDR r0, =current_thread
	MOV r1, #1
	STR r1, [r0]

	B end_if

switch_to_thread2:
	// Save thread1 sp
	MOV r1, sp
	LDR r0, =thread1_sp
	STR r1, [r0]

	// Switch to thread2 sp

	LDR r0, =thread2_sp
	LDR r1, [r0]
	MOV sp, r1

	// Set current_thread to 2
	LDR r0, =current_thread
	MOV r1, #2
	STR r1, [r0]

	B end_if

switch_to_thread1:
	// Save thread2 sp
	MOV r1, sp
	LDR r0, =thread2_sp
	STR r1, [r0]

	// Switch to thread1 sp

	LDR r0, =thread1_sp
	LDR r1, [r0]
	MOV sp, r1

	// Set current_thread to 1
	LDR r0, =current_thread
	MOV r1, #1
	STR r1, [r0]

	//B end_if

end_if:
	POP {r4-r11}

	//add r0, r0, r1
	CPSIE I
	bx lr
*/
