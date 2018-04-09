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
#include "SpdifPlayback.h"
#include <SRU.h>

/*==============  D E F I N E S  ===============*/

/*=============  D A T A  =============*/

/* Counter to keep track of number of ADC buffers processed */
//static volatile uint32_t SpdifPlayback::AsrcCount = 0u;
/* Counter to keep track of number of DAC buffers processed */
//static volatile uint32_t SpdifPlayback::DacCount = 0u;

/* DAC buffer pointer */
//static volatile void *SpdifPlayback::pGetASRC = NULL;
/* ADC buffer pointer */
//static volatile void *SpdifPlayback::pGetDAC = NULL;

//static void *SpdifPlayback::pASRC;
//static void *SpdifPlayback::pDAC;

/* Flag to register callback error */
//static volatile bool SpdifPlayback::bEventError = false;

/* Adc linear buffer that is divided into 2 sub buffers; ping and pong */
//ADI_CACHE_ALIGN static int8_t SpdifPlayback::AsrcBuf[ADI_CACHE_ROUND_UP_SIZE(AUDIO_BUFFER_SIZE * 2, int8_t)];

/* Dac linear buffer that is divided into 2 sub buffers; ping and pong  */
//static int8_t SpdifPlayback::DacBuf[AUDIO_BUFFER_SIZE * 2];


/*=============  L O C A L    F U N C T I O N    P R O T O T Y P E S =============*/

/* Process audio buffers */
//uint32_t 	ProcessBuffers(void);

/*=============  C A L L B A C K    F U N C T I O N    P R O T O T Y P E S =============*/

///* ASRC callback */
//void AsrcCallback(void *pCBParam, uint32_t nEvent, void *pArg);
///* DAC callback */
//void DacCallback(void *pCBParam, uint32_t nEvent, void *pArg);

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

	SpdifPlayback playback;
	playback.Run();


//    if(Result == 0u)
//    {
//    	bool run = true;
//        /* Audio loopback */
//        while(run)
//        {
//            /* Process audio buffers */
//        	playback.ProcessBuffers();
//            //Result = ProcessBuffers();
//
//            /* IF (Error) */
//            if(Result != 0u)
//            {
//                /* exit loopback */
//                break;
//            }
//
//            /* check if an error has been detected in callback */
//            if(SpdifPlayback::bEventError)
//            {
//                /* there has been an error returned in the callback */
//                Result =1u;
//                break;
//            }
//
//        }


//    }

//    if (Result == 0u)
//    {
//        printf("All done\n");
//    }
//    else
//    {
//        printf("Error\n");
//    }

    return 0;
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
//uint32_t ProcessBuffers(void)
//{
//    ADI_ASRC_RESULT         eResult1;
//    ADI_ADAU1962A_RESULT    eResult2;
//
//    uint32_t i =0u;
//    int8_t *pSrc;
//    int8_t *pDst;
//
//    /* re-submit the ASRC output buffer */
//    if(SpdifPlayback::pGetASRC != NULL)
//    {
//
//        /* DAC cannot be started until the first two ping-pong ASRC buffers are processed */
//        if(SpdifPlayback::AsrcCount == 2)
//        {
//            /* enable data flow */
//        	// TODO
//            if((uint32_t)adi_adau1962a_Enable(Adau1962Dac::phAdau1962a, true) != 0u)
//            {
//                /* return error */
//                return 1u;
//            }
//        }
//
//    	SpdifPlayback::pASRC = (void *)SpdifPlayback::pGetASRC;
//
//        /* submit the ADC buffer */
//        eResult1 = adi_asrc_OpSubmitBuffer(AsynchronousRateConverter::phAsrc0, (void *) SpdifPlayback::pASRC, AUDIO_BUFFER_SIZE);
//		if(eResult1)
//		{
//			return 1u;
//		}
//        SpdifPlayback::pGetASRC = NULL;
//    }
//
//    /* re-submit the DAC buffer */
//    if(SpdifPlayback::pGetDAC != NULL)
//    {
//        SpdifPlayback::pDAC = (void *)SpdifPlayback::pGetDAC;
//
//        /* submit the DAC buffer */
//        eResult2 = adi_adau1962a_SubmitBuffer(Adau1962Dac::phAdau1962a, (void *) SpdifPlayback::pDAC, AUDIO_BUFFER_SIZE);
//
//        SpdifPlayback::pGetDAC = NULL;
//    }
//
//    /* copy to DAC buffer */
//    if ((SpdifPlayback::pDAC != NULL) && (SpdifPlayback::pASRC != NULL))
//    {
//        pSrc =  (int8_t *)SpdifPlayback::pASRC;
//        pDst =  (int8_t *)SpdifPlayback::pDAC;
//
//        for(i=0; i<AUDIO_BUFFER_SIZE;i++)
//        {
//            *pDst++ = *pSrc++;
//
//        }
//
//        SpdifPlayback::pDAC = NULL;
//        SpdifPlayback::pASRC = NULL;
//
//    }
//
//    return (0u);
//}

///*
// * ADC Callback.
// *
// * Parameters
// *  None
// *
// * Returns
// *  None
// *
// */
//void AsrcCallback(void *pCBParam, uint32_t nEvent, void *pArg)
//{
//    switch(nEvent)
//    {
//        case ADI_SPORT_EVENT_RX_BUFFER_PROCESSED:
//            /* Update callback count */
//            SpdifPlayback::AsrcCount++;
//            /* store pointer to the processed buffer that caused the callback */
//            SpdifPlayback::pGetASRC = pArg;
//            break;
//        default:
//            SpdifPlayback::bEventError = true;
//            break;
//    }
//}

///*
// * DAC Callback.
// *
// * Parameters
// *  None
// *
// * Returns
// *  None
// *
// */
//void DacCallback(void *pCBParam, uint32_t nEvent, void *pArg)
//{
//    switch(nEvent)
//    {
//        case ADI_SPORT_EVENT_TX_BUFFER_PROCESSED:
//            /* Update callback count */
//            SpdifPlayback::DacCount++;
//            /* store pointer to the processed buffer that caused the callback */
//            SpdifPlayback::pGetDAC = pArg;
//            break;
//        default:
//            SpdifPlayback::bEventError = true;
//            break;
//    }
//}

/*****/
