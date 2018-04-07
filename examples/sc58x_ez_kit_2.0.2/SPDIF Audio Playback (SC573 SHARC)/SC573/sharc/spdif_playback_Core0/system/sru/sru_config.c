/*
 **
 ** sru_config.c source file generated on April 7, 2018 at 12:34:32.	
 **
 ** Copyright (C) 2014-2018 Analog Devices Inc., All Rights Reserved.
 **
 ** This file is generated automatically based upon the options selected in 
 ** the SRU Configuration editor. Changes to the SRU configuration should
 ** be made by changing the appropriate options rather than editing this file.
 **
 ** Only registers with non-default values are written to this file.
 **
 */
 
#include <stdint.h>
#include <sys/platform.h>

int32_t adi_SRU_Init(void);

/*
 * Initialize the Signal Routing Unit
 */
int32_t adi_SRU_Init(void)
{
    /* SPDIF0_TX_EXT_SYNC_I, PCG0_EXTCLKB_I, PCG0_SYNC_CLKA_I, PCG0_EXTCLKA_I, PCG0_SYNC_CLKB_I */
    *pREG_DAI0_CLK4 = (unsigned int) 0x3def7bc2;

    /* SPT2B_CLK_I, SPT0B_CLK_I, SPT1B_CLK_I, SPT1A_CLK_I, SPT0A_CLK_I, SPT2A_CLK_I */
    *pREG_DAI0_CLK0 = (unsigned int) 0x03e630dc;

    /* SRC0_CLK_OP_I, SRC1_CLK_IP_I, SRC2_CLK_IP_I, SRC2_CLK_OP_I, SRC0_CLK_IP_I, SRC1_CLK_OP_I */
    *pREG_DAI0_CLK1 = (unsigned int) 0x3def7b9a;

    /* SRC3_DAT_IP_I, SRC0_TDM_OP_I, SRC2_TDM_OP_I, SRC3_TDM_OP_I, SRC1_TDM_OP_I */
    *pREG_DAI0_DAT3 = (unsigned int) 0x3efbefbe;

    /* SPDIF0_TX_DAT_I */
    *pREG_DAI0_DAT4 = (unsigned int) 0x0000003e;

    /* SPDIF0_RX_I */
    *pREG_DAI0_DAT5 = (unsigned int) 0x08000000;

    /* SPT1A_D0_I, SPT0B_D0_I, SPT0B_D1_I, SPT0A_D0_I, SPT0A_D1_I */
    *pREG_DAI0_DAT0 = (unsigned int) 0x3e17efa0;

    /* SRC1_DAT_IP_I, SPT2B_D0_I, SPT2B_D1_I, SRC2_DAT_IP_I, SRC0_DAT_IP_I */
    *pREG_DAI0_DAT2 = (unsigned int) 0x3efa8444;

    /* SPT1A_FS_I, SPT2A_FS_I, SPT0B_FS_I, SPT2B_FS_I, SPT0A_FS_I, SPT1B_FS_I */
    *pREG_DAI0_FS0 = (unsigned int) 0x0736b4fc;

    /* SRC2_FS_IP_I, SRC0_FS_OP_I, SRC0_FS_IP_I, SRC1_FS_OP_I, SRC2_FS_OP_I, SRC1_FS_IP_I */
    *pREG_DAI0_FS1 = (unsigned int) 0x3def7b9a;

    /* DAI0_PB02_I, DAI0_PB01_I, DAI0_PB04_I, DAI0_PB03_I */
    *pREG_DAI0_PIN0 = (unsigned int) 0x0fdfbf1e;

    /* DAI0_PB18_I, DAI0_PB19_I, DAI0_INV_PB19_I, DAI0_PB17_I, DAI0_PB20_I, DAI0_INV_PB20_I */
    *pREG_DAI0_PIN4 = (unsigned int) 0x057f8ffe;

    /* DAI0_PB06_I, DAI0_PB05_I, DAI0_PB08_I, DAI0_PB07_I */
    *pREG_DAI0_PIN1 = (unsigned int) 0x04e84b9e;

    /* DAI0_PB12_I, DAI0_PB10_I, DAI0_PB11_I, DAI0_PB09_I */
    *pREG_DAI0_PIN2 = (unsigned int) 0x03668cfe;

    /* DAI0_PBEN07_I, DAI0_PBEN06_I, DAI0_PBEN09_I, DAI0_PBEN08_I, DAI0_PBEN10_I */
    *pREG_DAI0_PBEN1 = (unsigned int) 0x1300d30f;

    /* DAI0_PBEN03_I, DAI0_PBEN02_I, DAI0_PBEN01_I, DAI0_PBEN05_I, DAI0_PBEN04_I */
    *pREG_DAI0_PBEN0 = (unsigned int) 0x1e000001;


    return 0;
}

