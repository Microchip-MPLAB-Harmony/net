/*******************************************************************************
  MIIM Driver Device API for SAM GMAC devices

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_gmac.c

  Summary:
    MIIM driver device API for SAM platform - GMAC.

  Description:
    This file contains routines for MIIM driver register access.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2018-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "device.h"

#include "driver/miim/src/drv_miim_local.h"
#include "drv_miim_device.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************


//*****************************************************************************
/* MII Clock Selection

Summary:
Lists the possible speed selection for the Reduced Media Independent 
Interface (RMII).

Description:
This enumeration lists the possible speed selection for RMII.
The body contains only two states: RMII_10Mbps or RMII_100Mbps.

Remarks:
This enumeration is processor specific and is defined in the processor-
specific header files (see processor.h).
*/
typedef enum {

    GMAC_MIIM_SYSCLK_DIV_BY_8       = 0x00,
    GMAC_MIIM_SYSCLK_DIV_BY_16      = 0x01,
    GMAC_MIIM_SYSCLK_DIV_BY_32      = 0x02,
    GMAC_MIIM_SYSCLK_DIV_BY_48      = 0x03,
    GMAC_MIIM_SYSCLK_DIV_BY_64      = 0x04,
    GMAC_MIIM_SYSCLK_DIV_BY_96      = 0x05,
    GMAC_MIIM_SYSCLK_DIV_ABOVE_96   = 0x06,
} GMAC_MIIM_CLK;


static void _DRV_MIIM_SMIClockSet(uintptr_t miimId, uint32_t hostClock, uint32_t maxMIIMClock )
{
    uint32_t mdc_div; 
    GMAC_MIIM_CLK clock_dividor ; 
    mdc_div = hostClock / maxMIIMClock; 
    if (mdc_div <= 8 ) 
    { 
        clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_8; 
    } 
    else if (mdc_div <= 16 ) 
    { 
        clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_16; 
    } 
    else if (mdc_div <= 32 ) 
    { 
        clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_32; 
    } 
    else if (mdc_div <= 48 ) 
    { 
        clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_48; 
    } 
    else if (mdc_div <= 64 ) 
    { 
        clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_64; 
    } 
    else if (mdc_div <= 96 ) 
    { 
        clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_96; 
    } 
    else 
    { 
        clock_dividor = GMAC_MIIM_SYSCLK_DIV_ABOVE_96; 
    } 

    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) miimId;
    // disable tx if it is already enabled
    bool tx_stat = pGmacRegs->GMAC_NCR & GMAC_NCR_TXEN_Msk;
    if (tx_stat)
    {
        pGmacRegs->GMAC_NCR &= ~GMAC_NCR_TXEN_Msk; 
    }
    // disable rx if it is already enabled
    bool rx_stat = pGmacRegs->GMAC_NCR & GMAC_NCR_RXEN_Msk;
    if (rx_stat)
    {
        pGmacRegs->GMAC_NCR &= ~GMAC_NCR_RXEN_Msk;  
    }

    pGmacRegs->GMAC_NCFGR = (pGmacRegs->GMAC_NCFGR & 
            (~GMAC_NCFGR_CLK_Msk)) | 
        (clock_dividor << GMAC_NCFGR_CLK_Pos); 
    // enable tx if it was previously enabled
    if (tx_stat)
    {
        pGmacRegs->GMAC_NCR |= GMAC_NCR_TXEN_Msk; 
    }
    // enable rx if it was previously enabled
    if (rx_stat)
    {
        pGmacRegs->GMAC_NCR |= GMAC_NCR_RXEN_Msk;
    }

}

// ************** API implementation

// sets up the device: enable the device, sets the clock and other settings
DRV_MIIM_RESULT DRV_MIIM_DeviceSetup(uintptr_t miimId, const DRV_MIIM_SETUP* pSetUp)
{
    // setup the clock
    _DRV_MIIM_SMIClockSet(miimId, pSetUp->hostClockFreq, pSetUp->maxBusFreq );

    // other settings if needed
    return DRV_MIIM_RES_OK;    
} 

void DRV_MIIM_PortEnable(uintptr_t miimId)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) miimId;
    pGmacRegs->GMAC_NCR |=  GMAC_NCR_MPE_Msk;
}

void DRV_MIIM_PortDisable(uintptr_t miimId)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) miimId;
    pGmacRegs->GMAC_NCR &= ~GMAC_NCR_MPE_Msk;
}
    
bool DRV_MIIM_PortBusy(uintptr_t miimId)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) miimId;
    return ((pGmacRegs->GMAC_NSR & GMAC_NSR_IDLE_Msk) != GMAC_NSR_IDLE_Msk);
}

void DRV_MIIM_WriteData(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx, uint16_t wData)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) miimId;
    pGmacRegs->GMAC_MAN = 
        (~GMAC_MAN_WZO_Msk & GMAC_MAN_CLTTO_Msk)
        | (GMAC_MAN_OP(0x1)) 
        | GMAC_MAN_WTN(0x02) 
        | GMAC_MAN_PHYA(phyAdd) 
        | GMAC_MAN_REGA(regIx) 
        | GMAC_MAN_DATA(wData);


    // wait for device to be idle again...
    while((pGmacRegs->GMAC_NSR & GMAC_NSR_IDLE_Msk) == 0);

}

void DRV_MIIM_ReadStart(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) miimId;
    pGmacRegs->GMAC_MAN =    (~GMAC_MAN_WZO_Msk & GMAC_MAN_CLTTO_Msk) 
        | (GMAC_MAN_OP(0x2)) 
        | GMAC_MAN_WTN(0x02) 
        | GMAC_MAN_PHYA(phyAdd) 
        | GMAC_MAN_REGA(regIx) 
        | GMAC_MAN_DATA(0);

    // wait for device to be idle again...
    while((pGmacRegs->GMAC_NSR & GMAC_NSR_IDLE_Msk) == 0);
}

uint16_t DRV_MIIM_ReadDataGet(uintptr_t miimId)
{
    gmac_registers_t *  pGmacRegs = (gmac_registers_t *) miimId;
    return (uint16_t)(pGmacRegs->GMAC_MAN & GMAC_MAN_DATA_Msk) ;  
}

// scan not supported
DRV_MIIM_TXFER_STAT DRV_MIIM_ScanEnable(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx)
{   
    return DRV_MIIM_TXFER_ERROR;
}
    
// scan not supported
void DRV_MIIM_ScanDisable(uintptr_t miimId)
{ 
}

// scan not supported
bool DRV_MIIM_GetScanData(uintptr_t miimId, uint32_t* scanData)
{
    return false;
}


/*******************************************************************************
  End of File
*/

