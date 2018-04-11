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
    uint32_t Result = 0u;
    volatile uint32_t i;

    if((uint32_t)adi_gpio_Init((void*)GpioMemory, ADI_GPIO_CALLBACK_MEM_SIZE, &gpioMaxCallbacks) != 0u)
    {
        /* return error */
        //return 1u;
    }

    if((uint32_t)adi_gpio_SetDirection(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6, ADI_GPIO_DIRECTION_OUTPUT) != 0u)
    {
        /* return error */
        //return 1u;
    }
    /* bring reset low */
    if((uint32_t)adi_gpio_Clear(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6) != 0u)
    {
        /* return error */
       // return 1u;
    }

    /* delay */
    for (i = DELAY_COUNT; i ; i --);

    /* bring reset high */
    if((uint32_t)adi_gpio_Set(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6) != 0u)
    {
        /* return error */
        //return 1u;
    }

    /* delay */
    for (i = DELAY_COUNT; i ; i --);

}

GeneralPurposeIO::~GeneralPurposeIO() {
	// TODO Auto-generated destructor stub
}

void GeneralPurposeIO::CheckGpioResult(ADI_GPIO_RESULT expected, ADI_GPIO_RESULT result) {
	if ( result != expected )
	{
		char message[96];
		sprintf(message, "GeneralPurposeIO::CheckGpioResult expected(%d) != result(%d)", expected, result);
		perror(message);
		abort();
	}
}
