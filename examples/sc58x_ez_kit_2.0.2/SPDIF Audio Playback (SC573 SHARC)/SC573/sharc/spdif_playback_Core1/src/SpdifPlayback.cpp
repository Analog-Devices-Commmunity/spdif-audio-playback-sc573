/*
 * SpdifPlayback.cpp
 *
 *  Created on: 9 apr. 2018
 *      Author: more10
 */

#include "SpdifPlayback.h"

/*=============  C A L L B A C K    F U N C T I O N    P R O T O T Y P E S =============*/

/* ASRC callback */
void AsrcCallback(void *pCBParam, uint32_t nEvent, void *pArg);
/* DAC callback */
void DacCallback(void *pCBParam, uint32_t nEvent, void *pArg);

/* Counter to keep track of number of ADC buffers processed */
volatile uint32_t SpdifPlayback::AsrcCount = 0u;
/* Counter to keep track of number of DAC buffers processed */
volatile uint32_t SpdifPlayback::DacCount = 0u;

/* DAC buffer pointer */
volatile void* SpdifPlayback::pGetASRC = NULL;
/* ADC buffer pointer */
volatile void* SpdifPlayback::pGetDAC = NULL;

void *SpdifPlayback::pASRC;
void* SpdifPlayback::pDAC;

/* Flag to register callback error */
volatile bool SpdifPlayback::bEventError = false;

ADI_CACHE_ALIGN int8_t SpdifPlayback::AsrcBuf[ADI_CACHE_ROUND_UP_SIZE(AUDIO_BUFFER_SIZE * 2, int8_t)];

/* Dac linear buffer that is divided into 2 sub buffers; ping and pong  */
int8_t SpdifPlayback::DacBuf[AUDIO_BUFFER_SIZE * 2];


SpdifPlayback::SpdifPlayback()
: asrc(AsrcCallback),
  dac(DacCallback)
{
//    /* Enable the ASRC */
//    asrc.Enable();
//	/* Enable Rx SPDIF */
//    spdif.Enable();
//	/* Enable the PCG */
//	pcg.Enable();
}

SpdifPlayback::~SpdifPlayback() {
//    // close devices
//    spdif.Disable();
//    spdif.Close();
//
//    /* Disable and close DAC */
//    dac.Disable();
//    dac.Close();
//
//    /* Disable and close ASRC */
//    asrc.Disable();
//    asrc.Close();
//
//	/* Disable and close PCG */
//    pcg.Disable();
//    pcg.Close();
}

