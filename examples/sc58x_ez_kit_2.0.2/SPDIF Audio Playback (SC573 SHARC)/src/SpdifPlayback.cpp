/*
 * SpdifPlayback.cpp
 *
 *  Created on: 9 apr. 2018
 *      Author: more10
 */

#include "../include/spdif_playback.h"
#include "../include/SpdifPlayback.h"
#include <stdlib.h>
#include <stdio.h>

/*=============  C A L L B A C K    F U N C T I O N    P R O T O T Y P E S =============*/

///* ASRC callback */
//void AsrcCallback(void *pCBParam, uint32_t nEvent, void *pArg);
///* DAC callback */
//void DacCallback(void *pCBParam, uint32_t nEvent, void *pArg);


/* DAC buffer pointer */
volatile void* SpdifPlayback::pGetASRC = NULL;
/* ADC buffer pointer */
volatile void* SpdifPlayback::pGetDAC = NULL;

void *SpdifPlayback::pASRC;
void* SpdifPlayback::pDAC;

/* Flag to register callback error */
volatile bool SpdifPlayback::bEventError = false;

SpdifPlayback::SpdifPlayback()
{
    /* Submit ASRC buffers */
	asrc.AsrcSubmitBuffers();

    /* Submit DAC buffers */
	dac.Adau1962aSubmitBuffers();

    /* Enable the ASRC */
    asrc.Enable();

	/* Enable Rx SPDIF */
    spdif.Enable();

	/* Enable the PCG */
    pcg.Enable();
}

SpdifPlayback::~SpdifPlayback() {
    // close devices
    spdif.Disable();
    spdif.Close();

    /* Disable and close DAC */
    dac.Disable();
    dac.Close();

    /* Disable and close ASRC */
    asrc.Disable();
    asrc.Close();

	/* Disable and close PCG */
    pcg.Disable();
    pcg.Close();
}

void SpdifPlayback::Run() {
	bool run = true;
    /* Audio loopback */
    while(run)
    {
        /* Process audio buffers */
    	ProcessBuffers();

        /* check if an error has been detected in callback */
        if(bEventError)
        {
			abort();
        }

    }
}

void SpdifPlayback::ProcessBuffers(void) {
    ADI_ASRC_RESULT         eResult1;
    ADI_ADAU1962A_RESULT    eResult2;

    uint32_t i =0u;
    int8_t *pSrc;
    int8_t *pDst;

    /* re-submit the ASRC output buffer */
    if(SpdifPlayback::pGetASRC != NULL)
    {

        /* DAC cannot be started until the first two ping-pong ASRC buffers are processed */
        if(AsynchronousRateConverter::AsrcCount == 2)
        {
            /* enable data flow */
        	dac.Enable();

        }

    	SpdifPlayback::pASRC = (void *)SpdifPlayback::pGetASRC;

        /* submit the ADC buffer */
    	// TODO
        eResult1 = adi_asrc_OpSubmitBuffer(AsynchronousRateConverter::phAsrc0, (void *) SpdifPlayback::pASRC, AUDIO_BUFFER_SIZE);
		if(eResult1)
		{
			//return 1u;
			abort();
		}
        SpdifPlayback::pGetASRC = NULL;
    }

    /* re-submit the DAC buffer */
    if(SpdifPlayback::pGetDAC != NULL)
    {
        SpdifPlayback::pDAC = (void *)SpdifPlayback::pGetDAC;

        /* submit the DAC buffer */
        eResult2 = adi_adau1962a_SubmitBuffer(Adau1962Dac::phAdau1962a, (void *) SpdifPlayback::pDAC, AUDIO_BUFFER_SIZE);

        SpdifPlayback::pGetDAC = NULL;
    }

    /* copy to DAC buffer */
    if ((SpdifPlayback::pDAC != NULL) && (SpdifPlayback::pASRC != NULL))
    {
        pSrc =  (int8_t *)SpdifPlayback::pASRC;
        pDst =  (int8_t *)SpdifPlayback::pDAC;

        for(i=0; i<AUDIO_BUFFER_SIZE;i++)
        {
            *pDst++ = *pSrc++;

        }

        SpdifPlayback::pDAC = NULL;
        SpdifPlayback::pASRC = NULL;

    }
}

