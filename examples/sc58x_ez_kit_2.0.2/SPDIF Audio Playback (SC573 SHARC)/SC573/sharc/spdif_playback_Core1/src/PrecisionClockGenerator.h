/*
 * PrecisionClockGenerator.h
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#ifndef PRECISIONCLOCKGENERATOR_H_
#define PRECISIONCLOCKGENERATOR_H_

#include <services/pcg/adi_pcg.h>

class PrecisionClockGenerator {
private:
	/* PCG - A */
	static uint8_t PcgMemoryA[ADI_PCG_MEMORY_SIZE];
	static ADI_PCG_HANDLE phPcgA;
public:
	PrecisionClockGenerator();
	virtual ~PrecisionClockGenerator();
	void Enable();
	void Disable();
	void Close();
	static void CheckPcgResult(ADI_PCG_RESULT expected, ADI_PCG_RESULT actual);
};

#endif /* PRECISIONCLOCKGENERATOR_H_ */
