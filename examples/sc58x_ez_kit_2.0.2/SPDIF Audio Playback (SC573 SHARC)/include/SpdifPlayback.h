/*
 * SpdifPlayback.h
 *
 *  Created on: 9 apr. 2018
 *      Author: more10
 */

#ifndef SPDIFPLAYBACK_H_
#define SPDIFPLAYBACK_H_

#include <sys/platform.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "spdif_playback.h"
#include "adi_initialize.h"
#include "PowerService.h"
#include "SystemProtectionService.h"
#include "PrecisionClockGenerator.h"
#include "GeneralPurposeIO.h"
#include "SpdifDevice.h"
#include "AsynchronousRateConverter.h"
#include "Adau1962Dac.h"
#include <SRU.h>


class SpdifPlayback {
private:
	void ProcessBuffers(void);
public:
	/* Counter to keep track of number of ADC buffers processed */
	static volatile uint32_t AsrcCount;
	/* Counter to keep track of number of DAC buffers processed */
	static volatile uint32_t DacCount;

	/* DAC buffer pointer */
	static volatile void *pGetASRC;
	/* ADC buffer pointer */
	static volatile void *pGetDAC;

	static void *pASRC;
	static void *pDAC;

	/* Flag to register callback error */
	static volatile bool bEventError;

	// TODO flytta till AsynchronousRateConverter?
	/* Adc linear buffer that is divided into 2 sub buffers; ping and pong */
	ADI_CACHE_ALIGN static int8_t AsrcBuf[ADI_CACHE_ROUND_UP_SIZE(AUDIO_BUFFER_SIZE * 2, int8_t)];

	/* Dac linear buffer that is divided into 2 sub buffers; ping and pong  */
	static int8_t DacBuf[AUDIO_BUFFER_SIZE * 2];

    PowerService ps;

    /* Initialize SPU */
    SystemProtectionService sps;

	/* Initialize Precision Clock Generator */
    PrecisionClockGenerator pcg;

    /* Initialize GPIO for ADC/DAC reset control */
    GeneralPurposeIO gpio;

	/* Initialize SPDIF */
    SpdifDevice spdif;

    /* Initialize Asynchronous Rate Converter for the DAC*/
    AsynchronousRateConverter asrc;//(AsrcCallback);

    /* Initialize ADAU1962A */
    Adau1962Dac dac;//(DacCallback);
//public:
	SpdifPlayback();
	virtual ~SpdifPlayback();
	void Run();
};

#endif /* SPDIFPLAYBACK_H_ */
