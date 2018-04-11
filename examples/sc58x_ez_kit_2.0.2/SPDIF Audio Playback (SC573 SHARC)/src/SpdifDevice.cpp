/*
 * SpdifDevice.cpp
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#include "../include/SpdifDevice.h"
#include "../include/spdif_playback.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t SpdifDevice::RxSpdifMemory[ADI_SPDIF_RX_MEMORY_SIZE];
ADI_SPDIF_RX_HANDLE SpdifDevice::phRxSpdif;

SpdifDevice::SpdifDevice() {

    /* Open Rx SPDIF */
	ADI_SPDIF_RX_RESULT result = adi_spdif_Rx_Open(0u,
                          &RxSpdifMemory[0],
                          ADI_SPDIF_RX_MEMORY_SIZE,
                          &phRxSpdif);
	CheckSpdifResult(ADI_SPDIF_RX_SUCCESS, result);

    /*  rx auto restart*/
	result = adi_spdif_Rx_EnableAutoAudioRestart(phRxSpdif, true);
	CheckSpdifResult(ADI_SPDIF_RX_SUCCESS, result);

}

void SpdifDevice::Enable() {
	ADI_SPDIF_RX_RESULT result = adi_spdif_Rx_Enable(phRxSpdif, true);
	CheckSpdifResult(ADI_SPDIF_RX_SUCCESS, result);
}

void SpdifDevice::Disable() {
	ADI_SPDIF_RX_RESULT result = adi_spdif_Rx_Enable(phRxSpdif, false);
	CheckSpdifResult(ADI_SPDIF_RX_SUCCESS, result);
}

void SpdifDevice::Close() {
	ADI_SPDIF_RX_RESULT result = adi_spdif_Rx_Close(phRxSpdif);
	CheckSpdifResult(ADI_SPDIF_RX_SUCCESS, result);
}

SpdifDevice::~SpdifDevice() {
	// TODO Auto-generated destructor stub
}

void SpdifDevice::CheckSpdifResult(ADI_SPDIF_RX_RESULT expected, ADI_SPDIF_RX_RESULT result) {
	if ( result != expected )
	{
		char message[96];
		sprintf(message, "SpdifDevice::CheckSpdifResult expected(%d) != result(%d)", expected, result);
		perror(message);
		abort();
	}
}

