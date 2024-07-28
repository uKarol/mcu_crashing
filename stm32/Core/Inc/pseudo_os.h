/*
 * pseudo_os.h
 *
 *  Created on: Jul 28, 2024
 *      Author: Karol
 */

#ifndef INC_PSEUDO_OS_H_
#define INC_PSEUDO_OS_H_

#include <stdint.h>

typedef enum
{
	OS_INACTIVE,
	OS_ACTIVE,
}PseudoOS_Status_t;

typedef struct
{
	uint32_t *sp;
	uint32_t timeout;
	uint8_t priority;
}PseudoOS_Thread;

typedef void (*PseudoOS_ThreadHandler_t)(void);

void PseudoOS_Thread_Create(
	PseudoOS_Thread *thread,
	PseudoOS_ThreadHandler_t OS_ThreadHandler,
	void *stack_mem,
	uint32_t stack_size,
	uint8_t priority
	);

void PseudoOS_SwitchContext(void);
void PseudoOS_Start(void);
void PseudoOS_tick(void);
void PseudoOS_Delay(uint32_t timeout_milisecs);

extern volatile PseudoOS_Status_t Os_Status;

#endif /* INC_PSEUDO_OS_H_ */
