/*
 * AsynchronousRateConverter.cpp
 *
 *  Created on: 8 apr. 2018
 *      Author: more10
 */

#include "../include/AsynchronousRateConverter.h"
#include "../include/SpdifPlayback.h"
#include "../include/spdif_playback.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t AsynchronousRateConverter::AsrcMemory0[ADI_ASRC_MEMORY_SIZE];
ADI_ASRC_HANDLE AsynchronousRateConverter::phAsrc0;
/* ASRC SPORT configuration */
uint8_t AsynchronousRateConverter::OpAsrcSportMemory[ADI_SPORT_DMA_MEMORY_SIZE];
ADI_ASRC_SPORT_CONFIG AsynchronousRateConverter::OpAsrcSportConfig;

ADI_CACHE_ALIGN int8_t AsynchronousRateConverter::AsrcBuf[ADI_CACHE_ROUND_UP_SIZE(AUDIO_BUFFER_SIZE * 2, int8_t)];

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
        	//AsynchronousRateConverter::AsrcCount++;
            /* store pointer to the processed buffer that caused the callback */
            SpdifPlayback::pGetASRC = pArg;
            break;
        default:
            SpdifPlayback::bEventError = true;
            break;
    }
}

AsynchronousRateConverter::AsynchronousRateConverter() {
	// Initialize to 0 so that buffers can be used right away
	for (int i=0; i<AUDIO_BUFFER_SIZE * 2; i++) {
		AsynchronousRateConverter::AsrcBuf[i] = 0;
	}

	/* Open ASRC 0 */
	ADI_ASRC_RESULT result = adi_asrc_Open(0u,
    				  0u,
                      &AsrcMemory0[0],
                      ADI_ASRC_MEMORY_SIZE,
                      &phAsrc0);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);

    result = adi_asrc_SetSerialFormat(phAsrc0, ADI_ASRC_INPUT_I2S, ADI_ASRC_OUTPUT_I2S, ADI_ASRC_WORD_LENGTH_24);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);

    /* configure SPORT, this will handle Asrc0 */
    OpAsrcSportConfig.SportDevNum		= 0u;
    OpAsrcSportConfig.pSportDevMem 	    = &OpAsrcSportMemory[0u];
    OpAsrcSportConfig.SportDevMemSize 	= ADI_SPORT_DMA_MEMORY_SIZE;
    OpAsrcSportConfig.bLsbFirst			= false;
    OpAsrcSportConfig.bPack   			= false;
    OpAsrcSportConfig.bSecondaryEnabled	= false;
    OpAsrcSportConfig.eSportChnl		= ADI_ASRC_SPORT_A;

    /* Configure input ASRC sport channel */
    result = adi_asrc_OpConfigSport(phAsrc0, &OpAsrcSportConfig);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);

    /* Register output Sport callback */
    result = adi_asrc_OpRegisterSportCallback(phAsrc0, AsrcCallback, NULL);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);
}

AsynchronousRateConverter::~AsynchronousRateConverter() {
    /* Disable and close ASRC */
    Disable();
    Close();
}

void AsynchronousRateConverter::SubmitBuffer(void* buffer) {
	ADI_ASRC_RESULT eResult1;
	eResult1 = adi_asrc_OpSubmitBuffer(AsynchronousRateConverter::phAsrc0,
			buffer, AUDIO_BUFFER_SIZE);
	if (eResult1) {
		SpdifPlayback::bEventError = true;
	}
}

void AsynchronousRateConverter::AsrcSubmitBuffers(void) {
    /* submit ping buffer */
	ADI_ASRC_RESULT result = adi_asrc_OpSubmitBuffer(AsynchronousRateConverter::phAsrc0, &AsynchronousRateConverter::AsrcBuf[AUDIO_BUFFER_SIZE * 0u], AUDIO_BUFFER_SIZE);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);

    /* submit pong buffer */
	result = adi_asrc_OpSubmitBuffer(AsynchronousRateConverter::phAsrc0, &AsynchronousRateConverter::AsrcBuf[AUDIO_BUFFER_SIZE * 1u], AUDIO_BUFFER_SIZE);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);

}

void AsynchronousRateConverter::Enable() {
	ADI_ASRC_RESULT result = adi_asrc_Enable(phAsrc0, true);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);
}

void AsynchronousRateConverter::Disable() {
	ADI_ASRC_RESULT result = adi_asrc_Enable(phAsrc0, false);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);
}

void AsynchronousRateConverter::Close() {
	ADI_ASRC_RESULT result = adi_asrc_Close(phAsrc0);
	CheckAsrcResult(ADI_ASRC_SUCCESS, result);
}

void AsynchronousRateConverter::CheckAsrcResult(ADI_ASRC_RESULT expected, ADI_ASRC_RESULT result)
{
	if ( result != expected )
	{
		char message[96];
		sprintf(message, "AsynchronousRateConverter::CheckAsrcResult expected(%d) != result(%d)", expected, result);
		perror(message);
		abort();
	}
}
