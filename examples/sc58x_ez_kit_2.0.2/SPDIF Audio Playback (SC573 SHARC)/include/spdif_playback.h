/*********************************************************************************

Copyright(c) 2014-2015 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/
/*!
 * @file      spdif_playback.h
 * @brief     This example demonstrates a stereo uncompressed audio play-back by using the
 *            following components; a receive SPDIF interface, a Asynchronous Rate Converter
 *            and a ADAU1962a DAC.
 * @version:  $Revision: 29599 $
 * @date:     $Date: 2017-05-04 10:37:52 -0400 (Thu, 04 May 2017) $
 *
 * @details
 *            This is the primary source file for SPDIF audio play-back example.
 *
 */

#ifndef __SPDIF_PLAYBACK_H__
#define __SPDIF_PLAYBACK_H__

/*=============  I N C L U D E S   =============*/

/*==============  D E F I N E S  ===============*/

/* Number of callbacks before doing auto-correlation */
#define CALLBACK_COUNT  		(20000u)

/* Core clock is generated from CGU0 */
#define CGU_DEV                 (0)
#define CGU_DEV_1               (1)
#define MHZ                     (1000000u)
#define CLKIN                   (25u * MHZ)

/* Macro to specify delay count for ADC/DAC reset */
#define DELAY_COUNT             (100000u)

/*
 * DAC settings
 */
/* DAC Master clock frequency */
#define ADAU1962A_MCLK_IN       (24576000u)
/* DAC sample rate */
#define SAMPLE_RATE   			(192000u)

/* ADAU1962A SPORT config parameters */
#define LR_B_CLK_MASTER_1962    (true)
#define BCLK_RISING_1962 	    (true)
#define LRCLK_HI_LO_1962        (true)


/* Macro to set buffer size */
#define SAMPLE_SIZE 				(4u)
#define NUM_CHANNELS				(2u)
#define SAMPLES_PER_PERIOD_3K		(32u)
#define RESAMPLE_FACTOR				(4u)
#define SAMPLES_PER_PERIOD_12K		(SAMPLES_PER_PERIOD_3K*RESAMPLE_FACTOR)
#define SAMPLES_PER_PERIOD_48K		(SAMPLES_PER_PERIOD_12K*RESAMPLE_FACTOR)
#define SAMPLES_PER_PERIOD_192K		(SAMPLES_PER_PERIOD_48K*RESAMPLE_FACTOR)

#define SAMPLES_PER_PERIOD			(SAMPLES_PER_PERIOD_192K)
#define SAMPLE_BUFFER_SIZE 			(SAMPLE_SIZE * SAMPLES_PER_PERIOD)
#define AUDIO_BUFFER_SIZE 	        (SAMPLE_BUFFER_SIZE * NUM_CHANNELS)

/* SPU Peripheral ID */

#define	SPORT_0A_SPU_PID		    (ADI_SPU0_SPORT0A_ID)
#define	SPORT_2B_SPU_PID		    (ADI_SPU0_SPORT2B_ID)
#define	SPORT_0A_DMA10_SPU_PID		(ADI_SPU0_DMA0_ID)
#define	SPORT_2B_DMA11_SPU_PID		(ADI_SPU0_DMA5_ID)

#define PIN_MASK (0x0000007Fu)
#define PIN_LEN  (7u)

#define DATA_MASK (0x0000003Fu)
#define DATA_LEN  (6u)

#define CLK_MASK (0x0000001Fu)
#define CLK_LEN  (5u)

#define FS_MASK (0x0000001Fu)
#define FS_LEN  (5u)

#define PE_MASK (0x0000003Fu)
#define PE_LEN  (6u)

/* IF (Debug info enabled) */
#if defined(ENABLE_DEBUG_INFO)
#define DBG_MSG                     printf
#else
#define DBG_MSG(...)
#endif


#endif /* __SPDIF_PLAYBACK_H__ */
