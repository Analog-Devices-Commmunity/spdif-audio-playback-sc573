/*
 * SystemProtectionService.h
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#ifndef SYSTEMPROTECTIONSERVICE_H_
#define SYSTEMPROTECTIONSERVICE_H_

#include <services/spu/adi_spu.h>

class SystemProtectionService {
	/* Memory required for the SPU operation */
	static uint8_t SpuMemory[ADI_SPU_MEMORY_SIZE];

	/* SPU handle */
	static ADI_SPU_HANDLE hSpu;
public:
	SystemProtectionService();
	virtual ~SystemProtectionService();
	static void CheckSpuResult(ADI_SPU_RESULT expected, ADI_SPU_RESULT result);
};

#endif /* SYSTEMPROTECTIONSERVICE_H_ */
