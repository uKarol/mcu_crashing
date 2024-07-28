/*
 * pseudo_os.c
 *
 *  Created on: Jul 28, 2024
 *      Author: Karol
 */

#include "pseudo_os.h"
#include "stm32g474xx.h"
#include <stdlib.h>

#define MAX_TH_NUM  32+1
#define USED_TH_NUM  2

volatile PseudoOS_Status_t Os_Status = OS_INACTIVE;

PseudoOS_Thread *volatile current_thread = NULL; /* pointer is volatile - NOT non volatile pointer to volatile variable!!! */
PseudoOS_Thread *volatile next_thread = NULL;

PseudoOS_Thread *th_array[MAX_TH_NUM];
volatile uint32_t created_th_num = 0;
volatile uint32_t current_th_idx = 0;
volatile uint32_t os_th_ready_to_run = 0;

volatile uint8_t exceed_test;
void PseudoOS_Start(void)
{
	Os_Status = OS_ACTIVE;
	__disable_irq();
	PseudoOS_SwitchContext();
	__enable_irq();
	exceed_test++;

}

void PseudoOS_Delay(uint32_t timeout_milisecs)
{
	/* idle thread must not be blocked */
	if(current_th_idx > 0)
	{
		th_array[current_th_idx]->timeout = timeout_milisecs;
		os_th_ready_to_run &= ~(1<<(current_th_idx-1));
		__disable_irq();
		PseudoOS_SwitchContext();
		__enable_irq();
	}
}

void PseudoOS_tick(void)
{
	for(uint8_t ctr = 1; ctr < created_th_num; ctr++)
	{
		if(th_array[ctr]->timeout>0)
		{
			if(--(th_array[ctr]->timeout) == 0)
			{
				os_th_ready_to_run |= (1<<(ctr-1));

			}
		}
	}
}


void PseudoOS_SwitchContext(void)
{
	/* if all threads are blocked call IDLE thread */
	if(os_th_ready_to_run != 0)
	{
		/* incerment idx until you find first ready to run th */
		do
		{
			if(++current_th_idx == created_th_num)
			{
				current_th_idx = 1;
			}

		}while((os_th_ready_to_run & (1<<(current_th_idx-1))) == 0);
		next_thread = th_array[current_th_idx];
	}
	else
	{
		next_thread = th_array[0];
		current_th_idx = 0;
	}
	if(current_thread != next_thread)
	{
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; /* invoke pend sv */
	}
}


void PseudoOS_Thread_Create(
	PseudoOS_Thread *thread,
	PseudoOS_ThreadHandler_t OS_ThreadHandler,
	void *stack_mem,
	uint32_t stack_size
	)
{
	/* properly align stack pointer */
	uint32_t *stack_pointer = (uint32_t*)((((uint32_t)stack_mem + stack_size) / 8) * 8);

	uint32_t *stck_limit = (uint32_t*)(((((uint32_t)stack_mem - 1) / 8) + 1) * 8);

	*(--stack_pointer) = 1<<24; /* PSR THUMB STATE */
	*(--stack_pointer) = (uint32_t)(OS_ThreadHandler); /* PC */
	*(--stack_pointer) = 0x1; /* LR */
	*(--stack_pointer) = 0x2; /* R12 */
	*(--stack_pointer) = 0x3; /* R3 */
	*(--stack_pointer) = 0x4; /* R2 */
	*(--stack_pointer) = 0x5; /* R1 */
	*(--stack_pointer) = 0x6; /* R0 */
	  /* preserve next registers R11 - R4 */
	*(--stack_pointer) = 0x7; /* R11 */
	*(--stack_pointer) = 0x8; /* R10 */
	*(--stack_pointer) = 0x9; /* R9 */
	*(--stack_pointer) = 0x10; /* R8 */
	*(--stack_pointer) = 0x11; /* R7 */
	*(--stack_pointer) = 0x12; /* R6 */
	*(--stack_pointer) = 0x13; /* R5 */
	*(--stack_pointer) = 0x14; /* R4 */

	thread->sp = stack_pointer;

	/* fill the bottom of stack with this trash - to ensure that it is not overflowed */
	for(stack_pointer = stack_pointer-1 ; stack_pointer >= stck_limit; stack_pointer--)
	{
		*stack_pointer = 0xBADC0FEE;
	}
	if(created_th_num < MAX_TH_NUM)
	{
		if(created_th_num > 0)
		{
			os_th_ready_to_run |= (1<<(created_th_num-1));
		}
		th_array[created_th_num++] = thread;
	}

}


__attribute__ ((naked, weak))
void PseudoOS_PendSV_Handler(void)
{
	/*
	 * in case of LDR, value from address in Rn is loaded to target adress
	 */


  /* USER CODE BEGIN PendSV_IRQn 0 */
	 asm volatile(
		".global current_thread\n\t"
		".global next_thread			@ import .global variable from C symbols - for GCC compiler \n\t"
		"CPSID I 				 		@ disable interrupts - pseudo critical section \n\t"
		"LDR R1, =current_thread 		@ load address of current thread to R1 \n\t"
		"LDR R1, [R1, #0]				@ load value of current thread from address in R1 \n\t"
		"CBZ R1, first_iteration		@ check if R1 (current thread is not null ptr - in this case branch to label first iteration\n\t"
		"PUSH {R4-R11}					@ push registers R4-R11 to stack (R0-R3 and R12, PC LR SP are already protected by ISR \n\t"
		"LDR R1, =current_thread		@ load address of current thread to R1\n\t"
		"LDR R1, [R1, #0]				@ load address of current thread to R1\n\t"
		"STR SP, [R1, #0] 				@ store value of current thread structure in R1 \n\t"

		"first_iteration:				@ execute this code when current thread equals NULL \n\t"
		"LDR R1, =next_thread 			@ load address of next thread to R1 \n\t"
		"LDR R1, [R1, #0]				@ load value of next thread from address in R1 \n\t"
		"LDR SP, [R1, #0] 				@ load address of next thread to stack pointer - not stack pointer points to a thread stack \n\t"
		"LDR R1, =next_thread \n\t"
		"LDR R1, [R1, #0] \n\t"
		"LDR R2, =current_thread 		 \n\t"
		"STR R1, [R2, #0]				@ now current thread = next thread \n\t"
		"POP {R4-R11} 					@ restore registers from stack \n\t"
		"CPSIE I\n\t"
		"BX LR"
	 );

}

