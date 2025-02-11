/*******************************************************************************
  MIIM Driver Device API for SAM EMAC devices

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_emac.c

  Summary:
    MIIM driver device API for SAM platform - EMAC.

  Description:
    This file contains routines for MIIM driver register access.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2018-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

static void F_DRV_MIIM_SMIClockSet(uintptr_t miimId, uint32_t hostClock, uint32_t maxMIIMClock )
{ 
    emac_registers_t *  macRegs = (emac_registers_t *) miimId; // EMAC0_REGS or EMAC1_REGS
    uint32_t            tenRenSettings = macRegs->EMAC_NCR & (EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
    uint32_t            clockDivider;
    uint32_t            mdcDiv;

    mdcDiv = hostClock / maxMIIMClock;
    if( mdcDiv <= 8U)
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_8;
    }
    else if( mdcDiv <= 16U)
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_16;
    }
    else if( mdcDiv <= 32U)
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_32;
    }
    else
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_64;
    }
    if( tenRenSettings == 0U )
    {   // REN and TEN are both disabled
        macRegs->EMAC_NCFGR |= clockDivider;
    }
    else
    {   // changing TEN will cause emac DMA to reset to first queue
        macRegs->EMAC_NCR &= ~tenRenSettings;
        macRegs->EMAC_NCFGR |= clockDivider;
        macRegs->EMAC_NCR |= tenRenSettings;
    }   
} 

// ************** API implementation

// sets up the device: enable the device, sets the clock and other settings
DRV_MIIM_RESULT DRV_MIIM_DeviceSetup(uintptr_t miimId, const DRV_MIIM_SETUP* pSetUp)
{
    // setup the clock
    F_DRV_MIIM_SMIClockSet(miimId, pSetUp->hostClockFreq, pSetUp->maxBusFreq );

    // other settings if needed
    return DRV_MIIM_RES_OK;    
} 

void DRV_MIIM_PortEnable(uintptr_t miimId)
{
    emac_registers_t * macRegs = (emac_registers_t *) miimId; // EMAC0_REGS or EMAC1_REGS
    if( EMAC_NCR_MPE_Msk != (macRegs->EMAC_NCR & EMAC_NCR_MPE_Msk) )
    {   // once enabled should not toggle RE or TE 
        uint32_t tenRenSettings = macRegs->EMAC_NCR & (EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
        if( tenRenSettings  == 0U)
        {   // REN and TEN are both disabled
            macRegs->EMAC_NCR |=  EMAC_NCR_MPE_Msk;
        }
        else
        {   // changing TEN will cause emac DMA to reset to first queue
            macRegs->EMAC_NCR &= ~tenRenSettings;
            macRegs->EMAC_NCR |= EMAC_NCR_MPE_Msk;
            macRegs->EMAC_NCR |= tenRenSettings;
        }
    }
}

void DRV_MIIM_PortDisable(uintptr_t miimId)
{
}
    
bool DRV_MIIM_PortBusy(uintptr_t miimId)
{
    emac_registers_t * macRegs = (emac_registers_t *) miimId; // EMAC0_REGS or EMAC1_REGS
   return ((macRegs->EMAC_NSR & EMAC_NSR_IDLE_Msk) != EMAC_NSR_IDLE_Msk);
}

void DRV_MIIM_WriteData(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx, uint16_t wData)
{
    emac_registers_t * macRegs = (emac_registers_t *) miimId; // EMAC0_REGS or EMAC1_REGS
    macRegs->EMAC_MAN = EMAC_MAN_SOF( 1 )       // clause 22 start of frame
                        | EMAC_MAN_RW( 1 )      // write
                        | EMAC_MAN_PHYA( phyAdd )
                        | EMAC_MAN_REGA( regIx )
                        | EMAC_MAN_CODE( 2 )
                        | EMAC_MAN_DATA( wData )
                        ;
}

void DRV_MIIM_ReadStart(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx)
{
    emac_registers_t * macRegs = (emac_registers_t *) miimId; // EMAC0_REGS or EMAC1_REGS
    macRegs->EMAC_MAN = EMAC_MAN_SOF( 1 )       // clause 22 start of frame
                        | EMAC_MAN_RW( 2 )      // read
                        | EMAC_MAN_PHYA( phyAdd )
                        | EMAC_MAN_REGA( regIx )
                        | EMAC_MAN_CODE( 2 )
                        | EMAC_MAN_DATA( 0 )
                        ;
}

uint16_t DRV_MIIM_ReadDataGet(uintptr_t miimId)
{
    emac_registers_t * macRegs = (emac_registers_t *) miimId; // EMAC0_REGS or EMAC1_REGS
    return (uint16_t)(macRegs->EMAC_MAN & EMAC_MAN_DATA_Msk);
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

