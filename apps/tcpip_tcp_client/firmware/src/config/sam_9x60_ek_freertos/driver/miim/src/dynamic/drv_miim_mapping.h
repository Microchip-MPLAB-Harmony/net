/*******************************************************************************
  MIIM Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_local.h

  Summary:
    MIIM driver local declarations and definitions.

  Description:
    This file contains the MIIM driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2013-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

//DOM-IGNORE-END

#ifndef _DRV_MIIM_MAPPING_H
#define _DRV_MIIM_MAPPING_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "device.h"
// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

static  __inline__ DRV_MIIM_RESULT
__attribute__((always_inline)) _DRV_MIIM_ETH_ENABLE( uintptr_t ethPhyId )
{
    return DRV_MIIM_RES_OK;
}

static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_MII_RELEASE_RESET( uintptr_t ethPhyId )
{
}

static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_SETUP_PREAMBLE(
    uintptr_t               ethPhyId,
    const DRV_MIIM_SETUP *  pSetUp
    )
{
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_SCAN_INCREMENT(
    uintptr_t               ethPhyId,
    const DRV_MIIM_SETUP *  pSetUp
    )
{
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_MNGMNT_PORT_ENABLE( uintptr_t ethPhyId )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    if( EMAC_NCR_MPE_Msk != (macRegs->EMAC_NCR & EMAC_NCR_MPE_Msk) )
    {   // once enabled should not toggle RE or TE because that will
        uint32_t tenRenSettings = macRegs->EMAC_NCR & (EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
        if( !tenRenSettings )
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
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_MNGMNT_PORT_DISABLE( uintptr_t ethPhyId )
{
}
static  __inline__ bool
__attribute__((always_inline)) _DRV_MIIM_IS_BUSY( uintptr_t ethPhyId )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
   if ((macRegs->EMAC_NSR & EMAC_NSR_IDLE_Msk) != EMAC_NSR_IDLE_Msk)
       return true;
   else
       return false;
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_PHYADDR_SET(
    uintptr_t           ethPhyId,
    DRV_MIIM_OP_DCPT *  pOpDcpt
    )
{
}
static  __inline__ DRV_MIIM_TXFER_STAT
__attribute__((always_inline)) _DRV_MIIM_OP_SCAN_ENABLE( uintptr_t ethPhyId )
{
     return DRV_MIIM_TXFER_SCAN_STALE;
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_OP_WRITE_DATA(
    uintptr_t           ethPhyId,
    DRV_MIIM_OP_DCPT *  pOpDcpt
    )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    macRegs->EMAC_MAN = EMAC_MAN_SOF( 1 )       // clause 22 start of frame
                        | EMAC_MAN_RW( 1 )      // write
                        | EMAC_MAN_PHYA( pOpDcpt->phyAdd )
                        | EMAC_MAN_REGA( pOpDcpt->regIx )
                        | EMAC_MAN_CODE( 2 )
                        | EMAC_MAN_DATA( pOpDcpt->opData )
                        ;
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_WRITE_START( uintptr_t ethPhyId )
{
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_OP_READ_START(
    uintptr_t           ethPhyId,
    DRV_MIIM_OP_DCPT *  pOpDcpt
    )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    macRegs->EMAC_MAN = EMAC_MAN_SOF( 1 )       // clause 22 start of frame
                        | EMAC_MAN_RW( 2 )      // read
                        | EMAC_MAN_PHYA( pOpDcpt->phyAdd )
                        | EMAC_MAN_REGA( pOpDcpt->regIx )
                        | EMAC_MAN_CODE( 2 )
                        | EMAC_MAN_DATA( pOpDcpt->opData )
                        ;
}
static  __inline__ uint16_t
__attribute__((always_inline)) _DRV_MIIM_OP_READ_DATA_GET( uintptr_t ethPhyId )
{
    emac_registers_t * macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    return (uint16_t)(macRegs->EMAC_MAN & EMAC_MAN_DATA_Msk);
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_CLEAR_DATA_VALID( uintptr_t ethPhyId )
{
}
static  __inline__ bool
__attribute__((always_inline)) _DRV_MIIM_IS_DATA_VALID( uintptr_t ethPhyId )
{
    return false;
}
static  __inline__ void
__attribute__((always_inline)) _DRV_MIIM_SCAN_DISABLE( uintptr_t ethPhyId )
{
}
static  __inline__ void __attribute__((always_inline)) _DRV_MIIM_SMI_CLOCK_SET(uintptr_t ethPhyId, uint32_t hostClock, uint32_t maxMIIMClock )
{ 
	emac_registers_t *  macRegs = (emac_registers_t *) ethPhyId; // EMAC0_REGS or EMAC1_REGS
    uint32_t            tenRenSettings = macRegs->EMAC_NCR & (EMAC_NCR_RE_Msk | EMAC_NCR_TE_Msk);
    uint32_t            clockDivider;
    uint32_t            mdcDiv;
    mdcDiv = hostClock / maxMIIMClock;
    if( mdcDiv <= 8 )
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_8;
    }
    else if( mdcDiv <= 16 )
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_16;
    }
    else if( mdcDiv <= 32 )
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_32;
    }
    else
    {
        clockDivider = EMAC_NCFGR_CLK_MCK_64;
    }
    if( !tenRenSettings )
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



#endif //#ifndef _DRV_MIIM_MAPPING_H

/*******************************************************************************
 End of File
*/

