/*
 * PowerService.cpp
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#include "../include/PowerService.h"

#include "../include/spdif_playback.h"
#include <stdio.h>
#include <stdlib.h>

void PowerService::CheckPwrResult(ADI_PWR_RESULT expected, ADI_PWR_RESULT result)
{
	if ( result != expected )
	{
		char message[96];
		sprintf(message, "PowerService::CheckPwrResult expected(%d) != result(%d)", expected, result);
		perror(message);
		abort();
	}
}


PowerService::PowerService() {
	/* For CCLK=450MHz, CGU0 is used to generate the OCLK_0 of 225MHz. */

	/* Initialize the power service to CLKIN=25MHz. This API is called so that we can use other low level APIs to
	 * modify individual clocks. adi_pwr_SetFreq() is not called.*/
	ADI_PWR_RESULT apr = adi_pwr_Init(CGU_DEV, CLKIN);
	CheckPwrResult(ADI_PWR_SUCCESS, apr);

	/* For S/PDIF sampling frequencies (24 - 96kHz),Configure CLK05 to between 170 MHz to 180 MHz.
	 * For S/PDIF sampling frequencies (32 -192kHz),Configure CLK05 to 225 MHz.*/

	/* Use OCLK_0 to get S/PDIF-Rx (CDU_CLKO5) freq of 225MHz. */
	apr = adi_pwr_SetClkDivideRegister(CGU_DEV, ADI_PWR_CLK_DIV_OSEL, 2);
	CheckPwrResult(ADI_PWR_SUCCESS, apr);

	/* Use OCLK_0 to get S/PDIF-Rx (CDU_CLKO5) freq of 225MHz. */
	apr = adi_pwr_ConfigCduInputClock(ADI_PWR_CDU_CLKIN_0, ADI_PWR_CDU_CLKOUT_5);
	CheckPwrResult(ADI_PWR_SUCCESS, apr);
}

PowerService::~PowerService() {
	// TODO Auto-generated destructor stub
}

