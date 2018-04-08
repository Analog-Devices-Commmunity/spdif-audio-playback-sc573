/*
 * Adau1962Dac.cpp
 *
 *  Created on: 8 apr. 2018
 *      Author: more10
 */

#include "Adau1962Dac.h"
#include "../../../../spdif_playback.h"
#include <stdio.h>
#include <stdlib.h>


/* ADAU1962A DAC DATA */
uint8_t Adau1962Dac::Adau1962aMemory[ADI_ADAU1962A_MEMORY_SIZE];
/* ADAU1962A Sport */
uint8_t Adau1962Dac::Adau1962aSportMemory[ADI_SPORT_DMA_MEMORY_SIZE];

ADI_ADAU1962A_HANDLE Adau1962Dac::phAdau1962a;

Adau1962Dac::Adau1962Dac(void *pTwiDevMem, ADI_CALLBACK pfCallback) {
    ADI_ADAU1962A_RESULT        eResult;
    ADI_ADAU1962A_TWI_CONFIG    TwiConfig;
    ADI_ADAU1962A_SPORT_CONFIG  SportConfig;

    /* Open ADAU1962A device instance */
    ADI_ADAU1962A_RESULT result = adi_adau1962a_Open(0u,
                                     ADI_ADAU1962A_SERIAL_MODE_STEREO,
                                     &Adau1962aMemory,
                                     ADI_ADAU1962A_MEMORY_SIZE,
                                     &phAdau1962a);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);

    /* TWI parameters required to open/configure TWI */
    TwiConfig.TwiDevNum     = 0u;
    TwiConfig.eTwiAddr      = ADI_ADAU1962A_TWI_ADDR_04;
    TwiConfig.TwiDevMemSize = ADI_TWI_MEMORY_SIZE;
    TwiConfig.pTwiDevMem    = pTwiDevMem;

    /* Configure TWI */
    result = adi_adau1962a_ConfigTwi (phAdau1962a, &TwiConfig);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);

    /* SPORT parameters required to open/configure SPORT */
    SportConfig.SportDevNum     = 2u;
    SportConfig.eSportChnl      = ADI_ADAU1962A_SPORT_B;
    SportConfig.eSportPri       = ADI_ADAU1962A_SERIAL_PORT_DSDATA1;
    SportConfig.eSportSec       = ADI_ADAU1962A_SERIAL_PORT_NONE;
    SportConfig.SportDevMemSize = ADI_SPORT_DMA_MEMORY_SIZE;
    SportConfig.pSportDevMem    = &Adau1962aSportMemory;

    /* Configure SPORT */
    result = adi_adau1962a_ConfigSport (phAdau1962a, &SportConfig);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);

    /* DAC Master Power-up */
    result = adi_adau1962a_ConfigDacPwr (phAdau1962a,
                                               ADI_ADAU1962A_CHNL_DAC_MSTR,
                                               ADI_ADAU1962A_DAC_PWR_LOW,
                                               true);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);

    /*
     * Configure PLL clock - DAC is clock master and drives SPORT clk and FS
     * MCLK 24.576 MHz and PLL uses MCLK
     */
    result = adi_adau1962a_ConfigPllClk (phAdau1962a,
                                               ADAU1962A_MCLK_IN,
                                               ADI_ADAU1962A_MCLK_SEL_PLL,
                                               ADI_ADAU1962A_PLL_IN_MCLKI_XTALI);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);

    /*
     * Configure serial data clock
     * DAC as clock master, External BCLK, Latch on raising edge
     * LRCLK at 50% duty cycle, MSB first, Left channel at LRCLK low
     */
    result = adi_adau1962a_ConfigSerialClk (phAdau1962a,
                                                  LR_B_CLK_MASTER_1962,
                                                  false,
                                                  BCLK_RISING_1962,
                                                  false,
                                                  false,
                                                  LRCLK_HI_LO_1962);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);
    /* Power-up PLL */
    result = adi_adau1962a_ConfigBlockPwr (phAdau1962a,
                                                 false,
                                                 true,
                                                 true);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);

    /* Configure Sample rate */
    result = adi_adau1962a_SetSampleRate (phAdau1962a, SAMPLE_RATE * 1u);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);

    /* Configure Word width */
    result = adi_adau1962a_SetWordWidth (phAdau1962a,
                                               ADI_ADAU1962A_WORD_WIDTH_24);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);

    /* Register callback */
    result = adi_adau1962a_RegisterCallback (phAdau1962a,
    												pfCallback,
                                                   NULL);
    CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);
}

Adau1962Dac::~Adau1962Dac() {
	// TODO Auto-generated destructor stub
}


void Adau1962Dac::Enable() {
	ADI_ADAU1962A_RESULT result = adi_adau1962a_Enable(phAdau1962a, true);
	CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);
}

void Adau1962Dac::Disable() {
	ADI_ADAU1962A_RESULT result = adi_adau1962a_Enable(phAdau1962a, false);
	CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);
}

void Adau1962Dac::Close() {
	ADI_ADAU1962A_RESULT result = adi_adau1962a_Close(phAdau1962a);
	CheckAdau1962aResult(ADI_ADAU1962A_SUCCESS, result);
}


void Adau1962Dac::CheckAdau1962aResult(ADI_ADAU1962A_RESULT expected, ADI_ADAU1962A_RESULT result) {
	if ( result != expected )
	{
		char message[96];
		sprintf(message, "SpdifDevice::CheckSpdifResult expected(%d) != result(%d)", expected, result);
		perror(message);
		abort();
	}
}
