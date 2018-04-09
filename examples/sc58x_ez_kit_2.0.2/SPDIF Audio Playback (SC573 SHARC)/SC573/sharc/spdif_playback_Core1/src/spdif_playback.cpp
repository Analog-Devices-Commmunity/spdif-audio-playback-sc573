/*********************************************************************************

Copyright(c) 2014-2016 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/
/*!
 * @file      spdif_loopback.c
 * @brief     This example demonstrates a stereo uncompressed audio play-back by using the
 *            following components; a receive SPDIF interface, a Asynchronous Rate Converter
 *            and a ADAU1962a DAC.
 * @version:  $Revision: 29734 $
 * @date:     $Date: 2017-05-15 01:41:55 -0400 (Mon, 15 May 2017) $
 *
 * @details
 *            This is the primary source file for SPDIF audio play-back example.
 *
 */

/*=============  I N C L U D E S   =============*/


#include <sys/platform.h>
/* SPU Manager includes */
#include <drivers/dac/adau1962a/adi_adau1962a.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../../../../spdif_playback.h"
#include "adi_initialize.h"
#include "PowerService.h"
#include "SystemProtectionService.h"
#include "PrecisionClockGenerator.h"
#include "GeneralPurposeIO.h"
#include "SpdifDevice.h"
#include "AsynchronousRateConverter.h"
#include "Adau1962Dac.h"
#include <SRU.h>

/*==============  D E F I N E S  ===============*/

/*=============  D A T A  =============*/

/* Twi  */
static uint8_t TwiMemory[ADI_TWI_MEMORY_SIZE];

/* Counter to keep track of number of ADC buffers processed */
static volatile uint32_t AsrcCount = 0u;
/* Counter to keep track of number of DAC buffers processed */
static volatile uint32_t DacCount = 0u;

/* DAC buffer pointer */
static volatile void *pGetASRC = NULL;
/* ADC buffer pointer */
static volatile void *pGetDAC = NULL;

static void *pASRC;
static void *pDAC;

/* Flag to register callback error */
static volatile bool bEventError = false;

/* Adc linear buffer that is divided into 2 sub buffers; ping and pong */
ADI_CACHE_ALIGN static int8_t AsrcBuf[ADI_CACHE_ROUND_UP_SIZE(AUDIO_BUFFER_SIZE * 2, int8_t)];

/* Dac linear buffer that is divided into 2 sub buffers; ping and pong  */
static int8_t DacBuf[AUDIO_BUFFER_SIZE * 2];


/*=============  L O C A L    F U N C T I O N    P R O T O T Y P E S =============*/

/* Submit buffers to ASRC */
uint32_t    AsrcSubmitBuffers(void);
/* Submit buffers to DAC */
uint32_t    Adau1962aSubmitBuffers(void);
/* Process audio buffers */
uint32_t 	ProcessBuffers(void);

/*=============  C A L L B A C K    F U N C T I O N    P R O T O T Y P E S =============*/

/* ASRC callback */
void AsrcCallback(void *pCBParam, uint32_t nEvent, void *pArg);
/* DAC callback */
void DacCallback(void *pCBParam, uint32_t nEvent, void *pArg);

/*=============  C O D E  =============*/

/*
 * Main function
 */
int main()
{
    /**
     * Initialize managed drivers and/or services that have been added to
     * the project.
     * @return zero on success
     */
    uint32_t Result = 0u;
    uint32_t i;

    /* configure System Event Controller SEC and Signal Routing Unit SRU */
    adi_initComponents();

    /* Initialize power service */
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
    AsynchronousRateConverter asrc(AsrcCallback);


    /* Initialize ADAU1962A */
    Adau1962Dac dac(&TwiMemory, DacCallback);

    /* Submit ASRC buffers */
    if(Result == 0u)
    {
        Result = AsrcSubmitBuffers();
    }

    /* Submit DAC buffers */
    if(Result == 0u)
    {
        Result = Adau1962aSubmitBuffers();
    }

    /* Enable the ASRC */
    asrc.Enable();

	/* Enable Rx SPDIF */
    spdif.Enable();

	/* Enable the PCG */
	pcg.Enable();

    if(Result == 0u)
    {
    	bool run = true;
        /* Audio loopback */
        while(run)
        {
            /* Process audio buffers */
            Result = ProcessBuffers();

            /* IF (Error) */
            if(Result != 0u)
            {
                /* exit loopback */
                break;
            }

            /* check if an error has been detected in callback */
            if(bEventError)
            {
                /* there has been an error returned in the callback */
                Result =1u;
                break;
            }

        }

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

    if (Result == 0u)
    {
        printf("All done\n");
    }
    else
    {
        printf("Error\n");
    }

    return 0;
}

/*
 * Submits ping-pong buffers
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t AsrcSubmitBuffers(void)
{
    uint32_t Result = 0u;

    /* submit ping buffer */
    if((uint32_t)adi_asrc_OpSubmitBuffer(AsynchronousRateConverter::phAsrc0, &AsrcBuf[AUDIO_BUFFER_SIZE * 0u], AUDIO_BUFFER_SIZE) != 0u)
    {
        /* return error */
        return 1u;
    }

    /* submit pong buffer */
    if((uint32_t)adi_asrc_OpSubmitBuffer(AsynchronousRateConverter::phAsrc0, &AsrcBuf[AUDIO_BUFFER_SIZE * 1u], AUDIO_BUFFER_SIZE) != 0u)
    {
        /* return error */
        return 1u;
    }

    return Result;
}

/*
 * Submits ping-pong buffers .
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t Adau1962aSubmitBuffers(void)
{
    uint32_t Result = 0u;

    /* submit ping buffer */
    if((uint32_t)adi_adau1962a_SubmitBuffer(Adau1962Dac::phAdau1962a, &DacBuf[AUDIO_BUFFER_SIZE * 0u], AUDIO_BUFFER_SIZE) != 0u)
    {
        /* return error */
        return 1u;
    }

    /* submit pong buffer */
    if((uint32_t)adi_adau1962a_SubmitBuffer(Adau1962Dac::phAdau1962a, &DacBuf[AUDIO_BUFFER_SIZE * 1u], AUDIO_BUFFER_SIZE) != 0u)
    {
        /* return error */
        return 1u;
    }

    return Result;
}

/*
 * Processes audio buffers that are processed by ADC and DAC driver.
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t ProcessBuffers(void)
{
    ADI_ASRC_RESULT         eResult1;
    ADI_ADAU1962A_RESULT    eResult2;

    uint32_t i =0u;
    int8_t *pSrc;
    int8_t *pDst;

    /* re-submit the ASRC output buffer */
    if(pGetASRC != NULL)
    {

        /* DAC cannot be started until the first two ping-pong ASRC buffers are processed */
        if(AsrcCount == 2)
        {
            /* enable data flow */
        	// TODO
            if((uint32_t)adi_adau1962a_Enable(Adau1962Dac::phAdau1962a, true) != 0u)
            {
                /* return error */
                return 1u;
            }
        }

    	pASRC = (void *)pGetASRC;

        /* submit the ADC buffer */
        eResult1 = adi_asrc_OpSubmitBuffer(AsynchronousRateConverter::phAsrc0, (void *) pASRC, AUDIO_BUFFER_SIZE);
		if(eResult1)
		{
			return 1u;
		}
        pGetASRC = NULL;
    }

    /* re-submit the DAC buffer */
    if(pGetDAC != NULL)
    {
        pDAC = (void *)pGetDAC;

        /* submit the DAC buffer */
        eResult2 = adi_adau1962a_SubmitBuffer(Adau1962Dac::phAdau1962a, (void *) pDAC, AUDIO_BUFFER_SIZE);

        pGetDAC = NULL;
    }

    /* copy to DAC buffer */
    if ((pDAC != NULL) && (pASRC != NULL))
    {
        pSrc =  (int8_t *)pASRC;
        pDst =  (int8_t *)pDAC;

        for(i=0; i<AUDIO_BUFFER_SIZE;i++)
        {
            *pDst++ = *pSrc++;

        }

        pDAC = NULL;
        pASRC = NULL;

    }

    return (0u);
}

/*
 * ADC Callback.
 *
 * Parameters
 *  None
 *
 * Returns
 *  None
 *
 */
void AsrcCallback(void *pCBParam, uint32_t nEvent, void *pArg)
{
    switch(nEvent)
    {
        case ADI_SPORT_EVENT_RX_BUFFER_PROCESSED:
            /* Update callback count */
            AsrcCount++;
            /* store pointer to the processed buffer that caused the callback */
            pGetASRC = pArg;
            break;
        default:
            bEventError = true;
            break;
    }
}

/*
 * DAC Callback.
 *
 * Parameters
 *  None
 *
 * Returns
 *  None
 *
 */
void DacCallback(void *pCBParam, uint32_t nEvent, void *pArg)
{
    switch(nEvent)
    {
        case ADI_SPORT_EVENT_TX_BUFFER_PROCESSED:
            /* Update callback count */
            DacCount++;
            /* store pointer to the processed buffer that caused the callback */
            pGetDAC = pArg;
            break;
        default:
            bEventError = true;
            break;
    }
}

/*****/
