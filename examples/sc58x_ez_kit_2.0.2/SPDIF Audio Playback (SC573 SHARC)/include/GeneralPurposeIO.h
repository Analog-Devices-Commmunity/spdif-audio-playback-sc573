/*
 * GeneralPurposeIO.h
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#ifndef GENERALPURPOSEIO_H_
#define GENERALPURPOSEIO_H_

#include <services/gpio/adi_gpio.h>
#include <string>

class GeneralPurposeIO {
public:
	GeneralPurposeIO();
	virtual ~GeneralPurposeIO();
private:
	static uint8_t GpioMemory[ADI_GPIO_CALLBACK_MEM_SIZE];
	static uint32_t gpioMaxCallbacks;

	static void CheckGpioResult(ADI_GPIO_RESULT expected, ADI_GPIO_RESULT result, const char* method);
};

#endif /* GENERALPURPOSEIO_H_ */
