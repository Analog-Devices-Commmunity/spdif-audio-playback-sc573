/*
 * Adau1962Dac.h
 *
 *  Created on: 8 apr. 2018
 *      Author: more10
 */

#ifndef ADAU1962DAC_H_
#define ADAU1962DAC_H_

#include <drivers/dac/adau1962a/adi_adau1962a.h>
#include "spdif_playback.h"

class Adau1962Dac {
private:
	/* ADAU1962A DAC DATA */
	static uint8_t Adau1962aMemory[ADI_ADAU1962A_MEMORY_SIZE];
	/* ADAU1962A Sport */
	static uint8_t Adau1962aSportMemory[ADI_SPORT_DMA_MEMORY_SIZE];
	/* Twi  */
	static uint8_t TwiMemory[ADI_TWI_MEMORY_SIZE];
	/* Dac linear buffer that is divided into 2 sub buffers; ping and pong  */
	static int8_t DacBuf[AUDIO_BUFFER_SIZE * 2];
public:
	static ADI_ADAU1962A_HANDLE phAdau1962a;
	/* Counter to keep track of number of DAC buffers processed */
	//static volatile uint32_t DacCount;

	Adau1962Dac();
	virtual ~Adau1962Dac();
	void Adau1962aSubmitBuffers(void);
	void Enable();
	void Disable();
	void Close();
	static void CheckAdau1962aResult(ADI_ADAU1962A_RESULT expected, ADI_ADAU1962A_RESULT result);

};

#endif /* ADAU1962DAC_H_ */
