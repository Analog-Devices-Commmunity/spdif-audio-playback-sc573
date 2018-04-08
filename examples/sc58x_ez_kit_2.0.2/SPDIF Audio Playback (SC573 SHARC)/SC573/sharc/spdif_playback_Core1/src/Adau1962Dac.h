/*
 * Adau1962Dac.h
 *
 *  Created on: 8 apr. 2018
 *      Author: more10
 */

#ifndef ADAU1962DAC_H_
#define ADAU1962DAC_H_

#include <drivers/dac/adau1962a/adi_adau1962a.h>

class Adau1962Dac {
private:
	/* ADAU1962A DAC DATA */
	static uint8_t Adau1962aMemory[ADI_ADAU1962A_MEMORY_SIZE];
	/* ADAU1962A Sport */
	static uint8_t Adau1962aSportMemory[ADI_SPORT_DMA_MEMORY_SIZE];
public:
	static ADI_ADAU1962A_HANDLE phAdau1962a;

	Adau1962Dac(void *pTwiDevMem, ADI_CALLBACK pfCallback);
	virtual ~Adau1962Dac();
	void Enable();
	void Disable();
	void Close();
	static void CheckAdau1962aResult(ADI_ADAU1962A_RESULT expected, ADI_ADAU1962A_RESULT result);

};

#endif /* ADAU1962DAC_H_ */
