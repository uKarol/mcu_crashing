/*
 * hw_abstraction.c
 *
 *  Created on: Jul 28, 2024
 *      Author: Karol
 */

#include "hw_abstraction.h"
#include "stm32g474xx.h"
#include  "project_config.h"

void HwAbstraction_LedSetup(void)
{
	GPIOA->MODER &= ~(GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
	GPIOA->MODER |= (GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);
}

void HwAbstraction_RedLedOn(void)
{
	RED_LED_GPIO->BSRR = RED_LED_PIN;
}

void HwAbstraction_RedLedOff(void)
{
	RED_LED_GPIO->BRR = RED_LED_PIN;
}

void HwAbstraction_BlueLedOn(void)
{
	BLUE_LED_GPIO->BSRR = BLUE_LED_PIN;
}

void HwAbstraction_BlueLedOff(void)
{
	BLUE_LED_GPIO->BRR = BLUE_LED_PIN;
}
