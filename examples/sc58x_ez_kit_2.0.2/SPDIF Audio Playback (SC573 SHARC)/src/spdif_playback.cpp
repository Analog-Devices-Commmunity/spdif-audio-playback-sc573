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
#include "../include/spdif_playback.h"
#include "adi_initialize.h"
#include "../include/SpdifPlayback.h"
#include <SRU.h>

/*=============  C O D E  =============*/

/*
 * Main function
 */
int main()
{

    /* configure System Event Controller SEC and Signal Routing Unit SRU */
    adi_initComponents();

	SpdifPlayback playback;
	playback.Run();

    return 0;
}


/*****/
