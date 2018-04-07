/*
 * SystemProtectionService.cpp
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#include <stddef.h>
#include "SystemProtectionService.h"
#include "../../../../spdif_playback.h"
#include <stdio.h>
#include <stdlib.h>

/* Memory required for the SPU operation */
uint8_t SystemProtectionService::SpuMemory[ADI_SPU_MEMORY_SIZE];
/* SPU handle */
ADI_SPU_HANDLE SystemProtectionService::hSpu;

///* Memory required for the SPU operation */
//static uint8_t SpuMemory[ADI_SPU_MEMORY_SIZE];
//
///* SPU handle */
//static ADI_SPU_HANDLE hSpu;

void SystemProtectionService::CheckSpuResult(ADI_SPU_RESULT expected, ADI_SPU_RESULT result)
{
	if ( result != expected )
	{
		char message[96];
		sprintf(message, "SystemProtectionService::CheckSpuResult expected(%d) != result(%d)", expected, result);
		perror(message);
		abort();
	}
}


SystemProtectionService::SystemProtectionService() {
    /* Initialize SPU Service */
	ADI_SPU_RESULT result = adi_spu_Init(0u, SpuMemory, NULL, NULL, &hSpu);
	CheckSpuResult(ADI_SPU_SUCCESS, result);


    /* Make SPORT 0A to generate secure transactions */
	result = adi_spu_EnableMasterSecure(hSpu, SPORT_0A_SPU_PID, true);
	CheckSpuResult(ADI_SPU_SUCCESS, result);

    /* Make SPORT 2B to generate secure transactions */
	result = adi_spu_EnableMasterSecure(hSpu, SPORT_2B_SPU_PID, true);
	CheckSpuResult(ADI_SPU_SUCCESS, result);

    /* Make SPORT 0A DMA to generate secure transactions */
	result = adi_spu_EnableMasterSecure(hSpu, SPORT_0A_DMA10_SPU_PID, true);
	CheckSpuResult(ADI_SPU_SUCCESS, result);

    /* Make SPORT 2B DMA to generate secure transactions */
	result = adi_spu_EnableMasterSecure(hSpu, SPORT_2B_DMA11_SPU_PID, true);
	CheckSpuResult(ADI_SPU_SUCCESS, result);

}

SystemProtectionService::~SystemProtectionService() {
	// TODO Auto-generated destructor stub
}

