/*
 * SpdifDevice.h
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#ifndef SPDIFDEVICE_H_
#define SPDIFDEVICE_H_

#include <drivers/spdif/adi_spdif_rx.h>

class SpdifDevice {
public:
	SpdifDevice();
	virtual ~SpdifDevice();
	void Enable();

private:
	static uint8_t RxSpdifMemory[ADI_SPDIF_RX_MEMORY_SIZE];
	static ADI_SPDIF_RX_HANDLE phRxSpdif;
	void Disable();
	void Close();
	static void CheckSpdifResult(ADI_SPDIF_RX_RESULT expected, ADI_SPDIF_RX_RESULT result);
};

#endif /* SPDIFDEVICE_H_ */
