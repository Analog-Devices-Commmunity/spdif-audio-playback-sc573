/*********************************************************************************

Copyright(c) 2014-2016 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/
/*!
 * @file      spdif_loopback.c
 * @brief     This example demonstrates a stereo uncompressed audio play-back by using the
 *            following components; a receive SPDIF interface, a Asynchronous Rate Converter
 *            and a ADAU1962a DAC.
 * @version:  $Revision: 29734 $
 * @date:     $Date: 2017-05-15 01:41:55 -0400 (Mon, 15 May 2017) $
 *
 * @details
 *            This is the primary source file for SPDIF audio play-back example.
 *
 */

/*=============  I N C L U D E S   =============*/


#include <sys/platform.h>
/* SPU Manager includes */
#include <services/spu/adi_spu.h>
#include <services/pwr/adi_pwr.h>
#include <services/gpio/adi_gpio.h>
#include <drivers/spdif/adi_spdif_rx.h>
#include <drivers/dac/adau1962a/adi_adau1962a.h>
#include <drivers/asrc/adi_asrc.h>
#include <services/pcg/adi_pcg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "spdif_playback.h"
#include "adi_initialize.h"

#include <SRU.h>

/*==============  D E F I N E S  ===============*/

/*=============  D A T A  =============*/

/* Twi  */
static uint8_t TwiMemory[ADI_TWI_MEMORY_SIZE];
/* Gpio */
static uint8_t GpioMemory[ADI_GPIO_CALLBACK_MEM_SIZE];
static uint32_t gpioMaxCallbacks;

/* RxSpdif  */
static uint8_t RxSpdifMemory[1000u];
static ADI_SPDIF_RX_HANDLE phRxSpdif;

/* ADAU1962A DAC DATA */
static ADI_ADAU1962A_HANDLE phAdau1962a;
static uint8_t Adau1962aMemory[ADI_ADAU1962A_MEMORY_SIZE];
/* ADAU1962A Sport */
static uint8_t Adau1962aSportMemory[ADI_SPORT_DMA_MEMORY_SIZE];

/* Asrc */
static uint8_t AsrcMemory0[ADI_ASRC_MEMORY_SIZE];
static ADI_ASRC_HANDLE phAsrc0;

/* ASRC SPORT configuration */
static uint8_t OpAsrcSportMemory[ADI_SPORT_DMA_MEMORY_SIZE];
static ADI_ASRC_SPORT_CONFIG OpAsrcSportConfig;

/* PCG - A */
static uint8_t PcgMemoryA[ADI_PCG_MEMORY_SIZE];
static ADI_PCG_HANDLE phPcgA;

/* Counter to keep track of number of ADC buffers processed */
static volatile uint32_t AsrcCount = 0u;
/* Counter to keep track of number of DAC buffers processed */
static volatile uint32_t DacCount = 0u;

/* DAC buffer pointer */
static volatile void *pGetASRC = NULL;
/* ADC buffer pointer */
static volatile void *pGetDAC = NULL;

static void *pASRC;
static void *pDAC;

/* Flag to register callback error */
static volatile bool bEventError = false;

/* Adc linear buffer that is divided into 2 sub buffers; ping and pong */
ADI_CACHE_ALIGN static int8_t AsrcBuf[ADI_CACHE_ROUND_UP_SIZE(AUDIO_BUFFER_SIZE * 2, int8_t)];

/* Dac linear buffer that is divided into 2 sub buffers; ping and pong  */
static int8_t DacBuf[AUDIO_BUFFER_SIZE * 2];

#if defined(__ADSPBF707_FAMILY__) || defined(__ADSP215xx__)
/* Memory required for the SPU operation */
static uint8_t SpuMemory[ADI_SPU_MEMORY_SIZE];

/* SPU handle */
static ADI_SPU_HANDLE hSpu;
#endif

/*=============  L O C A L    F U N C T I O N    P R O T O T Y P E S =============*/
/* Initialize power service */
uint32_t    PowerServiceInit(void);
/* Initialize SPU */
uint32_t    SpuInit(void);
/* Initialize PCG */
uint32_t 	PcgInit(void);
/* Initialize ASRC on the side of Rx Spdif */
uint32_t    AsrcInit(void);
/* Initialize GPIO and reset peripherals */
uint32_t    GpioInit(void);
/* Initialize Rx Spdif peripherals */
uint32_t	SpdifInit(void);
/* Initializes DAC */
uint32_t    Adau1962aInit(void);
/* Submit buffers to ASRC */
uint32_t    AsrcSubmitBuffers(void);
/* Submit buffers to DAC */
uint32_t    Adau1962aSubmitBuffers(void);
/* Process audio buffers */
uint32_t 	ProcessBuffers(void);

/*=============  C A L L B A C K    F U N C T I O N    P R O T O T Y P E S =============*/

/* ASRC callback */
void AsrcCallback(void *pCBParam, uint32_t nEvent, void *pArg);
/* DAC callback */
void DacCallback(void *pCBParam, uint32_t nEvent, void *pArg);

/*=============  E X T E R N A L    F U N C T I O N    P R O T O T Y P E S =============*/

/* Configures soft switches */
extern void ConfigSoftSwitches(void);

/*=============  C O D E  =============*/

/*
 * Main function
 */
int main()
{
    /**
     * Initialize managed drivers and/or services that have been added to
     * the project.
     * @return zero on success
     */
    uint32_t Result = 0u;
    uint32_t i;

    /* configure System Event Controller SEC and Signal Routing Unit SRU */
    adi_initComponents();

    /* Software Switch Configuration for the EZ-Board */
    ConfigSoftSwitches();

    /* Initialize power service */
    if(Result == 0u)
    {
        Result = PowerServiceInit();
    }

    /* Initialize SPU */
    if(Result == 0u)
    {
        Result = SpuInit();
    }

	/* Initialize Precision Clock Generator */
	if(Result == 0u)
	{
		Result = PcgInit();
	}

    /* Initialize GPIO for ADC/DAC reset control */
    if(Result == 0u)
    {
        Result = GpioInit();
    }

	/* Initialize SPDIF */
	if(Result == 0u)
	{
		Result = SpdifInit();
	}

    /* Initialize Asynchronous Rate Converter for the DAC*/
    if(Result == 0u)
    {
        Result = AsrcInit();
    }

    /* Initialize ADAU1962A */
    if(Result == 0u)
    {
        Result = Adau1962aInit();
    }

    /* Submit ASRC buffers */
    if(Result == 0u)
    {
        Result = AsrcSubmitBuffers();
    }

    /* Submit DAC buffers */
    if(Result == 0u)
    {
        Result = Adau1962aSubmitBuffers();
    }

    /* Enable the ASRC */

    if(Result == 0u)
    {
        Result = (uint32_t)adi_asrc_Enable(phAsrc0, true);
    }

	/* Enable Rx SPDIF */
	if(Result == 0u)
	{
		Result = (uint32_t)adi_spdif_Rx_Enable(phRxSpdif, true);
	}

	/* Enable the PCG */
	if(Result == 0u)
	{
		Result = (uint32_t)adi_pcg_Enable(phPcgA, true);
	}

    if(Result == 0u)
    {
    	bool run = true;
        /* Audio loopback */
        while(run)
        {
            /* Process audio buffers */
            Result = ProcessBuffers();

            /* IF (Error) */
            if(Result != 0u)
            {
                /* exit loopback */
                break;
            }

            /* check if an error has been detected in callback */
            if(bEventError)
            {
                /* there has been an error returned in the callback */
                Result =1u;
                break;
            }

//            /* Callback count limit reached */
//            if (DacCount >CALLBACK_COUNT)
//            {
//
//				/* Disable and close SPDIF */
//				adi_spdif_Rx_Enable(phRxSpdif, false);
//				adi_spdif_Rx_Close(phRxSpdif);
//
//                /* Disable and close DAC */
//                adi_adau1962a_Enable(phAdau1962a, false);
//                adi_adau1962a_Close(phAdau1962a);
//
//                /* Disable and close ASRC */
//                adi_asrc_Enable(phAsrc0, false);
//                adi_asrc_Close(phAsrc0);
//
//				/* Disable and close PCG */
//				adi_pcg_Enable    (phPcgA, false);
//				adi_pcg_Close     (phPcgA);
//
//                /* while loop break */
//                break;
//            }
        }

        // close devices
		adi_spdif_Rx_Enable(phRxSpdif, false);
		adi_spdif_Rx_Close(phRxSpdif);

        /* Disable and close DAC */
        adi_adau1962a_Enable(phAdau1962a, false);
        adi_adau1962a_Close(phAdau1962a);

        /* Disable and close ASRC */
        adi_asrc_Enable(phAsrc0, false);
        adi_asrc_Close(phAsrc0);

		/* Disable and close PCG */
		adi_pcg_Enable    (phPcgA, false);
		adi_pcg_Close     (phPcgA);

    }

    if (Result == 0u)
    {
        printf("All done\n");
    }
    else
    {
        printf("Error\n");
    }

    return 0;
}

/*
 * Initializes power service
 */
uint32_t PowerServiceInit(void)
{
    uint32_t Result = 0u;


	/* For CCLK=450MHz, CGU0 is used to generate the OCLK_0 of 225MHz. */

	/* Initialize the power service to CLKIN=25MHz. This API is called so that we can use other low level APIs to
	 * modify individual clocks. adi_pwr_SetFreq() is not called.*/
	if((uint32_t)adi_pwr_Init(CGU_DEV, CLKIN) != 0u)
		{
			/* return error */
			return 1u;
		}

	/* For S/PDIF sampling frequencies (24 - 96kHz),Configure CLK05 to between 170 MHz to 180 MHz.
	 * For S/PDIF sampling frequencies (32 -192kHz),Configure CLK05 to 225 MHz.*/

	/* Use OCLK_0 to get S/PDIF-Rx (CDU_CLKO5) freq of 225MHz. */
	if((uint32_t)adi_pwr_SetClkDivideRegister(CGU_DEV, ADI_PWR_CLK_DIV_OSEL, 2) != 0u)
		{
			/* return error */
			return 1u;
		}
	/* Use OCLK_0 to get S/PDIF-Rx (CDU_CLKO5) freq of 225MHz. */
	if((uint32_t)adi_pwr_ConfigCduInputClock(ADI_PWR_CDU_CLKIN_0, ADI_PWR_CDU_CLKOUT_5) != 0u)
	{
		/* return error */
		return 1u;
	}

    return Result;
}

uint32_t SpuInit(void)
{
    uint32_t Result = 0u;

    /* Initialize SPU Service */
    if(adi_spu_Init(0u, SpuMemory, NULL, NULL, &hSpu) != ADI_SPU_SUCCESS)
    {
        DBG_MSG("Failed to initialize SPU service\n");
        return (ADI_SPU_FAILURE);
    }


    /* Make SPORT 0A to generate secure transactions */
    if(adi_spu_EnableMasterSecure(hSpu, SPORT_0A_SPU_PID, true) != ADI_SPU_SUCCESS)
    {
        DBG_MSG("Failed to enable Master secure for SPORT 0A\n");
        return (ADI_SPU_FAILURE);
    }

    /* Make SPORT 2B to generate secure transactions */
    if(adi_spu_EnableMasterSecure(hSpu, SPORT_2B_SPU_PID, true) != ADI_SPU_SUCCESS)
    {
        DBG_MSG("Failed to enable Master secure for SPORT 2B\n");
        return (ADI_SPU_FAILURE);
    }

    /* Make SPORT 0A DMA to generate secure transactions */
    if(adi_spu_EnableMasterSecure(hSpu, SPORT_0A_DMA10_SPU_PID, true) != ADI_SPU_SUCCESS)
    {
        DBG_MSG("Failed to enable Master secure for SPORT 0A DMA\n");
        return (ADI_SPU_FAILURE);
    }

    /* Make SPORT 2B DMA to generate secure transactions */
    if(adi_spu_EnableMasterSecure(hSpu, SPORT_2B_DMA11_SPU_PID, true) != ADI_SPU_SUCCESS)
    {
        DBG_MSG("Failed to enable Master secure for SPORT 2B DMA\n");
        return (ADI_SPU_FAILURE);
    }

    return (Result);
}

/*
 * Opens and initializes ASRC Device.
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t AsrcInit(void)
{
    uint32_t Result = 0u;

    /* Open ASRC 0 */
    if( adi_asrc_Open(0u,
    				  0u,
                      &AsrcMemory0[0],
                      ADI_ASRC_MEMORY_SIZE,
                      &phAsrc0) != ADI_ASRC_SUCCESS)
    {
        Result = 1u;
    }

    if( adi_asrc_SetSerialFormat(phAsrc0, ADI_ASRC_INPUT_I2S, ADI_ASRC_OUTPUT_I2S, ADI_ASRC_WORD_LENGTH_24) != ADI_ASRC_SUCCESS)
    {
        Result = 1u;
    }

    /* configure SPORT, this will handle Asrc0 */
    OpAsrcSportConfig.SportDevNum		= 0u;
    OpAsrcSportConfig.pSportDevMem 	    = &OpAsrcSportMemory[0u];
    OpAsrcSportConfig.SportDevMemSize 	= ADI_SPORT_DMA_MEMORY_SIZE;
    OpAsrcSportConfig.bLsbFirst			= false;
    OpAsrcSportConfig.bPack   			= false;
    OpAsrcSportConfig.bSecondaryEnabled	= false;
    OpAsrcSportConfig.eSportChnl		= ADI_ASRC_SPORT_A;

    /* Configure input ASRC sport channel */
    if( adi_asrc_OpConfigSport(phAsrc0, &OpAsrcSportConfig) != ADI_ASRC_SUCCESS)
    {
        Result = 1u;
    }

    /* Register output Sport callback */
    if(adi_asrc_OpRegisterSportCallback(phAsrc0, AsrcCallback, NULL) != ADI_ASRC_SUCCESS)
    {
        Result = 1u;
    }

    return Result;
}


/*
 * Initializes GPIO service
 * A GPIO line is used to control reset of the ADC and DAC devices
 */
uint32_t GpioInit(void)
{
    uint32_t Result = 0u;
    /* Loop variable */
    volatile uint32_t i;

    if((uint32_t)adi_gpio_Init((void*)GpioMemory, ADI_GPIO_CALLBACK_MEM_SIZE, &gpioMaxCallbacks) != 0u)
    {
        /* return error */
        return 1u;
    }

    if((uint32_t)adi_gpio_SetDirection(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6, ADI_GPIO_DIRECTION_OUTPUT) != 0u)
    {
        /* return error */
        return 1u;
    }
    /* bring reset low */
    if((uint32_t)adi_gpio_Clear(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6) != 0u)
    {
        /* return error */
        return 1u;
    }

    /* delay */
    for (i = DELAY_COUNT; i ; i --);

    /* bring reset high */
    if((uint32_t)adi_gpio_Set(ADI_GPIO_PORT_A, ADI_GPIO_PIN_6) != 0u)
    {
        /* return error */
        return 1u;
    }

    /* delay */
    for (i = DELAY_COUNT; i ; i --);

    return Result;
}

/*
 * Opens and initializes PCG Device.
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t PcgInit(void)
{
	uint32_t Result = 0u;

	/* Init PCG A
	 *
	 * Using ext clk 24.576MHz
	 *
	 * CLKA 3.076 MHz
	 * FSA	48 kHz
	 */
    /* Open PCG A */
    if( adi_pcg_Open(0u,
                     &PcgMemoryA[0],
                     ADI_PCG_MEMORY_SIZE,
                     &phPcgA) != ADI_PCG_SUCCESS)
    {
        Result = 1u;
    }

    /* Select FS and clock outputs for PCGA */
	if((uint32_t)adi_pcg_SelectOutput(phPcgA, ADI_PCG_OUT_CLK_FS) != 0u)
	{
		/* return error */
		return 1u;
	}

    /* Set the PCGA input clock source to external*/
	if((uint32_t)adi_pcg_ClockSource(phPcgA, ADI_PCG_CLK_EXT) != 0u)
	{
		/* return error */
		return 1u;
	}

    /* Set the PCGA input fs to external */
	if((uint32_t)adi_pcg_FrameSyncSource(phPcgA, ADI_PCG_FS_EXT) != 0u)
	{
		/* return error */
		return 1u;
	}

    /* Clock A 3.076 MHz */
	if((uint32_t)adi_pcg_ClockDivide(phPcgA, 0x8) != 0u)
	{
		/* return error */
		return 1u;
	}
    /* FS A 48 kHz */
	if((uint32_t)adi_pcg_FrameSyncDivide(phPcgA, 0x200) != 0u)
	{
		/* return error */
		return 1u;
	}


	return Result;
}

/*
 * Opens and initializes Tx and Rx SPDIF Device.
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t SpdifInit(void)
{
	uint32_t Result = 0u;
    /* Open Rx SPDIF */
    if( adi_spdif_Rx_Open(0u,
                          &RxSpdifMemory[0],
                          ADI_SPDIF_RX_MEMORY_SIZE,
                          &phRxSpdif) != ADI_SPDIF_RX_SUCCESS)
    {
        Result = 1u;
    }

    /*  rx auto restart*/
    if( adi_spdif_Rx_EnableAutoAudioRestart(phRxSpdif, true) != ADI_SPDIF_RX_SUCCESS)
    {
        Result = 1u;
    }

	return Result;
}

/*
 * Opens and initializes ADAU1962A DAC Device.
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t Adau1962aInit(void)
{
    ADI_ADAU1962A_RESULT        eResult;
    ADI_ADAU1962A_TWI_CONFIG    TwiConfig;
    ADI_ADAU1962A_SPORT_CONFIG  SportConfig;

    /* Open ADAU1962A device instance */
    if((eResult = adi_adau1962a_Open(0u,
                                     ADI_ADAU1962A_SERIAL_MODE_STEREO,
                                     &Adau1962aMemory,
                                     ADI_ADAU1962A_MEMORY_SIZE,
                                     &phAdau1962a)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to open ADAU1962A device instance, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /* TWI parameters required to open/configure TWI */
    TwiConfig.TwiDevNum     = 0u;
    TwiConfig.eTwiAddr      = ADI_ADAU1962A_TWI_ADDR_04;
    TwiConfig.TwiDevMemSize = ADI_TWI_MEMORY_SIZE;
    TwiConfig.pTwiDevMem    = &TwiMemory;

    /* Configure TWI */
    if ((eResult = adi_adau1962a_ConfigTwi (phAdau1962a, &TwiConfig)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to configure TWI, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /* SPORT parameters required to open/configure SPORT */
    SportConfig.SportDevNum     = 2u;
    SportConfig.eSportChnl      = ADI_ADAU1962A_SPORT_B;
    SportConfig.eSportPri       = ADI_ADAU1962A_SERIAL_PORT_DSDATA1;
    SportConfig.eSportSec       = ADI_ADAU1962A_SERIAL_PORT_NONE;
    SportConfig.SportDevMemSize = ADI_SPORT_DMA_MEMORY_SIZE;
    SportConfig.pSportDevMem    = &Adau1962aSportMemory;

    /* Configure SPORT */
    if ((eResult = adi_adau1962a_ConfigSport (phAdau1962a, &SportConfig)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to configure SPORT, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /* DAC Master Power-up */
    if ((eResult = adi_adau1962a_ConfigDacPwr (phAdau1962a,
                                               ADI_ADAU1962A_CHNL_DAC_MSTR,
                                               ADI_ADAU1962A_DAC_PWR_LOW,
                                               true)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to configure DAC power, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /*
     * Configure PLL clock - DAC is clock master and drives SPORT clk and FS
     * MCLK 24.576 MHz and PLL uses MCLK
     */
    if ((eResult = adi_adau1962a_ConfigPllClk (phAdau1962a,
                                               ADAU1962A_MCLK_IN,
                                               ADI_ADAU1962A_MCLK_SEL_PLL,
                                               ADI_ADAU1962A_PLL_IN_MCLKI_XTALI)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to configure PLL clock, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /*
     * Configure serial data clock
     * DAC as clock master, External BCLK, Latch on raising edge
     * LRCLK at 50% duty cycle, MSB first, Left channel at LRCLK low
     */
    if ((eResult = adi_adau1962a_ConfigSerialClk (phAdau1962a,
                                                  LR_B_CLK_MASTER_1962,
                                                  false,
                                                  BCLK_RISING_1962,
                                                  false,
                                                  false,
                                                  LRCLK_HI_LO_1962)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to configure serial data clock, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /* Power-up PLL */
    if ((eResult = adi_adau1962a_ConfigBlockPwr (phAdau1962a,
                                                 false,
                                                 true,
                                                 true)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to Power-up PLL, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /* Configure Sample rate */
    if ((eResult = adi_adau1962a_SetSampleRate (phAdau1962a, SAMPLE_RATE * 1u)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to configure Sample rate, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /* Configure Word width */
    if ((eResult = adi_adau1962a_SetWordWidth (phAdau1962a,
                                               ADI_ADAU1962A_WORD_WIDTH_24)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to configure word width, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    /* Register callback */
    if ((eResult = adi_adau1962a_RegisterCallback (phAdau1962a,
                                                   DacCallback,
                                                   NULL)) != ADI_ADAU1962A_SUCCESS)
    {
        printf ("ADAU1962A: Failed to register callback, Error Code: 0x%08X\n", eResult);
        /* return error */
        return 1u;
    }

    return 0u;
}

/*
 * Submits ping-pong buffers
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t AsrcSubmitBuffers(void)
{
    uint32_t Result = 0u;

    /* submit ping buffer */
    if((uint32_t)adi_asrc_OpSubmitBuffer(phAsrc0, &AsrcBuf[AUDIO_BUFFER_SIZE * 0u], AUDIO_BUFFER_SIZE) != 0u)
    {
        /* return error */
        return 1u;
    }

    /* submit pong buffer */
    if((uint32_t)adi_asrc_OpSubmitBuffer(phAsrc0, &AsrcBuf[AUDIO_BUFFER_SIZE * 1u], AUDIO_BUFFER_SIZE) != 0u)
    {
        /* return error */
        return 1u;
    }

    return Result;
}

/*
 * Submits ping-pong buffers .
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t Adau1962aSubmitBuffers(void)
{
    uint32_t Result = 0u;

    /* submit ping buffer */
    if((uint32_t)adi_adau1962a_SubmitBuffer(phAdau1962a, &DacBuf[AUDIO_BUFFER_SIZE * 0u], AUDIO_BUFFER_SIZE) != 0u)
    {
        /* return error */
        return 1u;
    }

    /* submit pong buffer */
    if((uint32_t)adi_adau1962a_SubmitBuffer(phAdau1962a, &DacBuf[AUDIO_BUFFER_SIZE * 1u], AUDIO_BUFFER_SIZE) != 0u)
    {
        /* return error */
        return 1u;
    }

    return Result;
}

/*
 * Processes audio buffers that are processed by ADC and DAC driver.
 *
 * Parameters
 *  None
 *
 * Returns
 *  0 if success, other values for error
 *
 */
uint32_t ProcessBuffers(void)
{
    ADI_ASRC_RESULT         eResult1;
    ADI_ADAU1962A_RESULT    eResult2;

    uint32_t i =0u;
    int8_t *pSrc;
    int8_t *pDst;

    /* re-submit the ASRC output buffer */
    if(pGetASRC != NULL)
    {

        /* DAC cannot be started until the first two ping-pong ASRC buffers are processed */
        if(AsrcCount == 2)
        {
            /* enable data flow */
            if((uint32_t)adi_adau1962a_Enable(phAdau1962a, true) != 0u)
            {
                /* return error */
                return 1u;
            }
        }

    	pASRC = (void *)pGetASRC;

        /* submit the ADC buffer */
        eResult1 = adi_asrc_OpSubmitBuffer(phAsrc0, (void *) pASRC, AUDIO_BUFFER_SIZE);
		if(eResult1)
		{
			return 1u;
		}
        pGetASRC = NULL;
    }

    /* re-submit the DAC buffer */
    if(pGetDAC != NULL)
    {
        pDAC = (void *)pGetDAC;

        /* submit the DAC buffer */
        eResult2 = adi_adau1962a_SubmitBuffer(phAdau1962a, (void *) pDAC, AUDIO_BUFFER_SIZE);

        pGetDAC = NULL;
    }

    /* copy to DAC buffer */
    if ((pDAC != NULL) && (pASRC != NULL))
    {
        pSrc =  (int8_t *)pASRC;
        pDst =  (int8_t *)pDAC;

        for(i=0; i<AUDIO_BUFFER_SIZE;i++)
        {
            *pDst++ = *pSrc++;

        }

        pDAC = NULL;
        pASRC = NULL;

    }

    return (0u);
}

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
            AsrcCount++;
            /* store pointer to the processed buffer that caused the callback */
            pGetASRC = pArg;
            break;
        default:
            bEventError = true;
            break;
    }
}

/*
 * DAC Callback.
 *
 * Parameters
 *  None
 *
 * Returns
 *  None
 *
 */
void DacCallback(void *pCBParam, uint32_t nEvent, void *pArg)
{
    switch(nEvent)
    {
        case ADI_SPORT_EVENT_TX_BUFFER_PROCESSED:
            /* Update callback count */
            DacCount++;
            /* store pointer to the processed buffer that caused the callback */
            pGetDAC = pArg;
            break;
        default:
            bEventError = true;
            break;
    }
}

/*****/
