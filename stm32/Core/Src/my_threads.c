/*
 * my_threads.c
 *
 *  Created on: Jul 28, 2024
 *      Author: Karol
 */

#include "pseudo_os.h"
#include "hw_abstraction.h"
#include "project_config.h"
#include "stm32g474xx.h"

PseudoOS_Thread red_th;
PseudoOS_Thread blue_th;
PseudoOS_Thread idle_th;

uint32_t idle_stack[PSEUDO_STACK_SIZE];
uint32_t blue_stack[PSEUDO_STACK_SIZE];
uint32_t red_stack[PSEUDO_STACK_SIZE];

void idle_task(void)
{

	while(1)
	{
		__WFI();
	}
}

void pseudo_task_red_led(void)
{
	while(1)
	{
		HwAbstraction_RedLedOn();
		PseudoOS_Delay(500);
		HwAbstraction_RedLedOff();
		PseudoOS_Delay(500);
	}
}

void pseudo_task_blue_led(void)
{
	while(1)
	{
		HwAbstraction_BlueLedOn();
		PseudoOS_Delay(200);
		HwAbstraction_BlueLedOff();
		PseudoOS_Delay(200);
	}
}
