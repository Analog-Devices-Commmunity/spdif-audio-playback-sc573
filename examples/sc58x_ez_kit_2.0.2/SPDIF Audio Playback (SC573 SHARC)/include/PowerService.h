/*
 * PowerService.h
 *
 *  Created on: 7 apr. 2018
 *      Author: more10
 */

#ifndef POWERSERVICE_H_
#define POWERSERVICE_H_

#include <services/pwr/adi_pwr.h>

class PowerService {
public:
	PowerService();
	virtual ~PowerService();
	static void CheckPwrResult(ADI_PWR_RESULT expected, ADI_PWR_RESULT result);
};

#endif /* POWERSERVICE_H_ */
