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
public:
	SpdifPlayback();
	virtual ~SpdifPlayback();
	void Run();
	static void SetErrorTrue();
	static void ProcessAsrcBuffer(void* buffer);

	/* ADC buffer pointer */
	static volatile void *pGetDAC;

private:
	void ProcessBuffers(void);

	/* DAC buffer pointer */
	static volatile void *pGetASRC;

	static void *pASRC;
	static void *pDAC;
	/* Flag to register callback error */
	static volatile bool bEventError;


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
};

#endif /* SPDIFPLAYBACK_H_ */
