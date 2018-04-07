/*******************************************************************************

Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you 
agree to the terms of the associated Analog Devices License Agreement.

*******************************************************************************/

/*
 *  Software Switch Configuration for the ADSP-SC573 EZ-BOARD version 0.2
 *
 *  Please see the SoftConfig documentation in CCES Help for detailed
 *  information on SoftConfig.
 *
 */

#include <drivers\twi\adi_twi.h>

/* TWI settings */
#define TWI_PRESCALE  (10u)
#define TWI_BITRATE   (100u)
#define TWI_DUTYCYCLE (50u)

#define BUFFER_SIZE (32u)

/* TWI hDevice handle */
static ADI_TWI_HANDLE hDevice;

/* TWI data buffer */
static uint8_t twiBuffer[BUFFER_SIZE];

/* TWI device memory */
static uint8_t deviceMemory[ADI_TWI_MEMORY_SIZE];

/* switch register structure */
typedef struct {
	uint8_t Register;
	uint8_t Value;
} SWITCH_CONFIG;

/* switch parameter structure */
typedef struct {
	uint32_t TWIDevice;
	uint16_t HardwareAddress;
	uint32_t NumConfigSettings;
	SWITCH_CONFIG *ConfigSettings;
} SOFT_SWITCH;

/* prototype */
void ConfigSoftSwitches(void);

/* disable misra diagnostics as necessary */
#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rule_8_7:"Objects shall be defined at block scope")
#pragma diag(suppress:misra_rule_17_4:"Array indexing shall be the only allowed form of pointer arithmetic")
#endif /* _MISRA_RULES */

#ifdef __ADSPGCC__
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

/************************SoftConfig Information********************************

    ~ means the signal is active low

    Please see the ADSP-SC573 EZ-BOARD manual for more information on using
    Software-Controlled Switches(SoftConfig)

********************************************************************************/

/* switch 0 register settings */
static SWITCH_CONFIG SwitchConfig0[] =
{

 /*
       U47 Port A                                  U47 Port B
  7--------------- ~CAN1_EN             |       7--------------- ~SPDIF_DIGITAL EN
  | 6------------- ~CAN0_EN             |       | 6------------- ~SPDIF_OPTICAL EN
  | | 5----------- ~MLB3_EN             |       | | 5----------- ~SPI2D2_D3_EN
  | | | 4--------- ~ETH1_EN             |       | | | 4--------- ~SPI2FLASH_CS_EN
  | | | | 3------- ~ETH0_EN             |       | | | | 3------- ~SD_WP_EN
  | | | | | 2----- ~UART0_EN            |       | | | | | 2----- AUDIO_JACK_SEL
  | | | | | | 1--- ~UART0_FLOW_EN       |       | | | | | | 1--- ~ADAU1979_EN
  | | | | | | | 0- ~EEPROM_EN           |       | | | | | | | 0- ~ADAU1962_EN
  | | | | | | | |                       |       | | | | | | | |
  N X N N N Y Y Y                       |       Y N N Y N Y Y Y     ( Active Y or N )
  0 0 0 0 0 0 0 0                       |       0 1 0 0 1 1 0 0     ( value being set )
*/
  { 0x12u, 0x00u },                               { 0x13u, 0x4Cu },

 /*
  * specify inputs/outputs
  */

  { 0x0u, 0x00u },   /* Set IODIRA direction (all output) */
  { 0x1u, 0x00u },   /* Set IODIRB direction (all output) */
};
/* switch 1 register settings */
static SWITCH_CONFIG SwitchConfig1[] =
{

/*
            U48 Port A                                    U48 Port B

    7--------------- ~FLG3_LOOP         |       7--------------- Not Used
    | 6------------- ~FLG2_LOOP         |       | 6------------- Not Used
    | | 5----------- ~FLG1_LOOP         |       | | 5----------- LINK_PORT_ISOLATE
    | | | 4--------- ~FLG0_LOOP         |       | | | 4--------- ~AD2410_MASTER_SLAVE
    | | | | 3------- ~LEDS_EN           |       | | | | 3------- ~ENGINE_RPM_OE
    | | | | | 2----- ~PUSHBUTTON1_EN    |       | | | | | 2----- ~THUMBWHEEL_OE
    | | | | | | 1--- ~PUSHBUTTON2_EN    |       | | | | | | 1--- ~ADAU1977_FAULT_RST_EN
    | | | | | | | 0- ~PUSHBUTTON3_EN    |       | | | | | | | 0- ~ADAU1977_EN
    | | | | | | | |                     |       | | | | | | | |
    X X X Y Y N Y N                     |       X X N X X X X X    ( Active Y or N )
    1 1 1 1 0 0 0 0                     |       1 1 0 1 1 1 1 1    ( value being set )
*/
  { 0x12u, 0xF0u },                               { 0x13u, 0xDFu },

  /*
   * specify inputs/outputs
   */

  { 0x0u, 0xE0u },    /* Set IODIRA direction (bit 7-5 input, all others output) */
  { 0x1u, 0xFFu },    /* Set IODIRB direction (all input) */
};

/* switch configuration */
static SOFT_SWITCH SoftSwitch[] =
{
  {
    0u,
    0x21u,
    sizeof(SwitchConfig0)/sizeof(SWITCH_CONFIG),
    SwitchConfig0
  },
  {
    0u,
    0x22u,
    sizeof(SwitchConfig1)/sizeof(SWITCH_CONFIG),
    SwitchConfig1
  }
};
     
#if defined(ADI_DEBUG)
#include <stdio.h>
#define CHECK_RESULT(result, message) \
	do { \
		if((result) != ADI_TWI_SUCCESS) \
		{ \
			printf((message)); \
			printf("\n"); \
		} \
	} while (0)  /* do-while-zero needed for Misra Rule 19.4 */
#else
#define CHECK_RESULT(result, message)
#endif
 
void ConfigSoftSwitches(void)
{
	uint32_t switchNum;
	uint32_t configNum;
	uint32_t switches;

	SOFT_SWITCH *ss;
	SWITCH_CONFIG *configReg;
	ADI_TWI_RESULT result;

	switches = (uint32_t)(sizeof(SoftSwitch)/sizeof(SOFT_SWITCH));
	for (switchNum=0u; switchNum<switches; switchNum++)
	{
		ss = &SoftSwitch[switchNum];

		result = adi_twi_Open(ss->TWIDevice, ADI_TWI_MASTER, 
    		deviceMemory, ADI_TWI_MEMORY_SIZE, &hDevice);
		CHECK_RESULT(result, "adi_twi_Open failed");

		result = adi_twi_SetHardwareAddress(hDevice, ss->HardwareAddress);
		CHECK_RESULT(result, "adi_twi_SetHardwareAddress failed");

		result = adi_twi_SetPrescale(hDevice, TWI_PRESCALE);
		CHECK_RESULT(result, "adi_twi_Prescale failed");

		result = adi_twi_SetBitRate(hDevice, TWI_BITRATE);
		CHECK_RESULT(result, "adi_twi_SetBitRate failed");

		result = adi_twi_SetDutyCycle(hDevice, TWI_DUTYCYCLE);
		CHECK_RESULT(result, "adi_twi_SetDutyCycle failed");
		
		/* switch register settings */
		for (configNum=0u; configNum<ss->NumConfigSettings; configNum++)
		{
			configReg = &ss->ConfigSettings[configNum];

			/* write register value */
			twiBuffer[0] = configReg->Register;
			twiBuffer[1] = configReg->Value;
			result = adi_twi_Write(hDevice, twiBuffer, (uint32_t)2, false);
			CHECK_RESULT(result, "adi_twi_Write failed");
		}

		result = adi_twi_Close(hDevice);
		CHECK_RESULT(result, "adi_twi_Close failed");
	}
}

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */
