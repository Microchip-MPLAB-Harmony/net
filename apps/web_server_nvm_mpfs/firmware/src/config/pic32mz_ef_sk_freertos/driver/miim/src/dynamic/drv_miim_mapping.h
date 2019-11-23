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
#include "driver/ethmac/src/dynamic/drv_eth_pic32_lib.h"



   static const short _MIIMClockDivisorTable[]=
    {
        4, 6, 8, 10, 14, 20, 28, 40, 
#if defined (__PIC32MZ__)
        48, 50,
#endif  // defined (__PIC32MZ__)
    };  // divider values for the Host clock
   
    static  __inline__ DRV_MIIM_RESULT __attribute__((always_inline))_DRV_MIIM_ETH_ENABLE(uintptr_t ethPhyId)
    {        
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_MIIM_RESULT res = DRV_MIIM_RES_OK;

        if(!DRV_ETH_IsEnabled(ethId)) 
        { 
            DRV_ETH_Enable(ethId);  
            res = DRV_MIIM_RES_INIT_WARNING;
        } 
        return res;
    }
    

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MII_RELEASE_RESET(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIResetDisable(ethId);

    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SETUP_PREAMBLE(uintptr_t ethPhyId, const DRV_MIIM_SETUP* pSetUp)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        if((pSetUp->setupFlags & DRV_MIIM_SETUP_FLAG_PREAMBLE_SUPPRESSED) != 0)
        {
            DRV_ETH_MIIMNoPreEnable(ethId);            
        }
        else
        {
            DRV_ETH_MIIMNoPreDisable(ethId);
        }

    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_INCREMENT(uintptr_t ethPhyId, const DRV_MIIM_SETUP* pSetUp)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        if((pSetUp->setupFlags & DRV_MIIM_SETUP_FLAG_SCAN_ADDRESS_INCREMENT) != 0)
        {
            DRV_ETH_MIIMScanIncrEnable(ethId);
        }
        else
        {
            DRV_ETH_MIIMScanIncrDisable(ethId);
        }

    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_ENABLE(uintptr_t ethPhyId)
    {
       
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_DISABLE(uintptr_t ethPhyId)
    {
       
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_BUSY(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        return DRV_ETH_MIIMIsBusy(ethId);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_PHYADDR_SET(uintptr_t ethPhyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_PHYAddressSet(ethId, pOpDcpt->phyAdd);
        DRV_ETH_RegisterAddressSet(ethId,pOpDcpt->regIx);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_CLEAR_DATA_VALID(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_ClearDataValid(ethId);
    }
   
    static  __inline__ DRV_MIIM_TXFER_STAT __attribute__((always_inline))_DRV_MIIM_OP_SCAN_ENABLE(uintptr_t ethPhyId)
    {   
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_ClearDataValid(ethId);
        DRV_ETH_MIIMScanModeEnable(ethId);
        return DRV_MIIM_TXFER_SCAN_STALE;
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_WRITE_DATA(uintptr_t ethPhyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIMWriteDataSet(ethId, pOpDcpt->opData);
    }
    

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_WRITE_START(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIMWriteStart(ethId);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_READ_START(uintptr_t ethPhyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIMReadStart(ethId);
    }

    static  __inline__ uint16_t __attribute__((always_inline))_DRV_MIIM_OP_READ_DATA_GET(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        return DRV_ETH_MIIMReadDataGet(ethId);
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_DATA_VALID(uintptr_t ethPhyId)
    {
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        return !DRV_ETH_DataNotValid(ethId);
    }
      
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_DISABLE(uintptr_t ethPhyId)
    { 
        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        DRV_ETH_MIIMScanModeDisable(ethId);
    }
    
    static  __inline__ void __attribute__((always_inline)) _DRV_MIIM_SMI_CLOCK_SET(uintptr_t ethPhyId, uint32_t hostClock, uint32_t maxMIIMClock )
    {

        DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)_ETH_BASE_ADDRESS;
        int  ix;
        DRV_ETH_MIIMResetEnable(ethId); // Issue MIIM reset
        DRV_ETH_MIIMResetDisable(ethId); // Clear MIIM reset

        for(ix = 0; ix < sizeof(_MIIMClockDivisorTable) / sizeof(*_MIIMClockDivisorTable); ix++)
        {
            if(hostClock / _MIIMClockDivisorTable[ix] <= maxMIIMClock)
            {   // found it
                break;
            }
        }

        if(ix == sizeof(_MIIMClockDivisorTable) / sizeof(*_MIIMClockDivisorTable))
        {
            ix--;   // max divider; best we can do
        }

        DRV_ETH_MIIMClockSet(ethId, ix + 1);  // program the clock*/

    }

#endif //#ifndef _DRV_MIIM_MAPPING_H

/*******************************************************************************
 End of File
*/

