/*
 * PrecisionClockGenerator.cpp
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#include "../include/PrecisionClockGenerator.h"
#include "../include/spdif_playback.h"
#include <stdio.h>
#include <stdlib.h>

/* PCG - A */
uint8_t PrecisionClockGenerator::PcgMemoryA[ADI_PCG_MEMORY_SIZE];
ADI_PCG_HANDLE PrecisionClockGenerator::phPcgA;

PrecisionClockGenerator::PrecisionClockGenerator() {
	/* Init PCG A
	 *
	 * Using ext clk 24.576MHz
	 *
	 * CLKA 12.288 MHz
	 * FSA	192 kHz
	 */
	/* Open PCG A */
	ADI_PCG_RESULT result = adi_pcg_Open(0u,
			&PcgMemoryA[0],
			ADI_PCG_MEMORY_SIZE,
			&phPcgA);
	CheckPcgResult(ADI_PCG_SUCCESS, result);

	/* Select FS and clock outputs for PCGA */
	result = adi_pcg_SelectOutput(phPcgA, ADI_PCG_OUT_CLK_FS);
	CheckPcgResult(ADI_PCG_SUCCESS, result);

	/* Set the PCGA input clock source to external*/
	result = adi_pcg_ClockSource(phPcgA, ADI_PCG_CLK_EXT);
	CheckPcgResult(ADI_PCG_SUCCESS, result);

	/* Set the PCGA input fs to external */
	result = adi_pcg_FrameSyncSource(phPcgA, ADI_PCG_FS_EXT);
	CheckPcgResult(ADI_PCG_SUCCESS, result);

	/* Clock A 12.288 MHz */
	result = adi_pcg_ClockDivide(phPcgA, 0x2);
	CheckPcgResult(ADI_PCG_SUCCESS, result);

	/* FS A 192 kHz */
	result = adi_pcg_FrameSyncDivide(phPcgA, 0x80);
	CheckPcgResult(ADI_PCG_SUCCESS, result);
}

void PrecisionClockGenerator::Enable() {
	ADI_PCG_RESULT result = adi_pcg_Enable(phPcgA, true);
	CheckPcgResult(ADI_PCG_SUCCESS, result);
}

void PrecisionClockGenerator::Disable() {
	ADI_PCG_RESULT result = adi_pcg_Enable(phPcgA, false);
	CheckPcgResult(ADI_PCG_SUCCESS, result);
}

void PrecisionClockGenerator::Close() {
	ADI_PCG_RESULT result = adi_pcg_Close(phPcgA);
	CheckPcgResult(ADI_PCG_SUCCESS, result);
}

PrecisionClockGenerator::~PrecisionClockGenerator() {
	// TODO Auto-generated destructor stub
}

void PrecisionClockGenerator::CheckPcgResult(ADI_PCG_RESULT expected, ADI_PCG_RESULT actual) {
	if ( actual != expected )
	{
		char message[96];
		sprintf(message, "PrecisionClockGenerator::CheckPcgResult expected(%d) != result(%d)", expected, actual);
		perror(message);
		abort();
	}
}
