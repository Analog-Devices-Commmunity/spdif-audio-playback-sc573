/*
 * GeneralPurposeIO.cpp
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#include "../include/GeneralPurposeIO.h"
#include "../include/spdif_playback.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t GeneralPurposeIO::GpioMemory[ADI_GPIO_CALLBACK_MEM_SIZE];
uint32_t GeneralPurposeIO::gpioMaxCallbacks;

GeneralPurposeIO::GeneralPurposeIO() {
    volatile uint32_t i;

    ADI_GPIO_RESULT result = adi_gpio_Init((void*)GpioMemory, ADI_GPIO_CALLBACK_MEM_SIZE, &gpioMaxCallbacks);
    CheckGpioResult(ADI_GPIO_SUCCESS, result, "Constructor. adi_gpio_Init");

    result = adi_gpio_SetDirection(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6, ADI_GPIO_DIRECTION_OUTPUT);
    CheckGpioResult(ADI_GPIO_SUCCESS, result, "Constructor. adi_gpio_SetDirection");

    /* bring reset low */
    result = adi_gpio_Clear(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6);
    CheckGpioResult(ADI_GPIO_SUCCESS, result, "Constructor. adi_gpio_Clear");

    /* delay */
    for (i = DELAY_COUNT; i ; i --);

    /* bring reset high */
    result = adi_gpio_Set(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6);

    /* delay */
    for (i = DELAY_COUNT; i ; i --);

}

GeneralPurposeIO::~GeneralPurposeIO() {
}

void GeneralPurposeIO::CheckGpioResult(ADI_GPIO_RESULT expected, ADI_GPIO_RESULT result, const char* method) {
	if ( result != expected )
	{
		char message[96];
		sprintf(message, "GeneralPurposeIO::%s expected(%d) != result(%d)", method, expected, result);
		perror(message);
		abort();
	}
}
