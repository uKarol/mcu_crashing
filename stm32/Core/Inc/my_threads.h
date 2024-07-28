/*
 * my_threads.h
 *
 *  Created on: Jul 28, 2024
 *      Author: Karol
 */

#ifndef INC_MY_THREADS_H_
#define INC_MY_THREADS_H_

#include "pseudo_os.h"
#include "project_config.h"

extern PseudoOS_Thread red_th;
extern PseudoOS_Thread blue_th;
extern PseudoOS_Thread idle_th;

extern uint32_t idle_stack[PSEUDO_STACK_SIZE];
extern uint32_t blue_stack[PSEUDO_STACK_SIZE];
extern uint32_t red_stack[PSEUDO_STACK_SIZE];

void idle_task(void);
void pseudo_task_red_led(void);
void pseudo_task_blue_led(void);

__attribute__ ((naked, weak))
void PseudoOS_PendSV_Handler(void);
#endif /* INC_MY_THREADS_H_ */
