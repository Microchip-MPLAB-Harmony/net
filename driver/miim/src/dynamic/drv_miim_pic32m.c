/*******************************************************************************
  MIIM Driver Device API for PIC32M

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_pic32m.c

  Summary:
    MIIM driver device API for PIC32M platform.

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
#include "driver/ethmac/src/dynamic/drv_eth_pic32_lib.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************



static const uint16_t _MIIMClockDivisorTable[]=
{
    4, 6, 8, 10, 14, 20, 28, 40, 
#if defined (__PIC32MZ__)
    48, 50,
#endif  // defined (__PIC32MZ__)
};  // divider values for the Host clock



static DRV_MIIM_RESULT _DRV_MIIM_Enable(uintptr_t miimId)
{        
    DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)miimId;
    DRV_MIIM_RESULT res = DRV_MIIM_RES_OK;

    if(!DRV_ETH_IsEnabled(ethId)) 
    { 
        DRV_ETH_Enable(ethId);  
        res = DRV_MIIM_RES_INIT_WARNING;
    } 
    DRV_ETH_MIIResetDisable(ethId);
    return res;
}
    
static  void _DRV_MIIM_SetupPreamble(uintptr_t miimId, const DRV_MIIM_SETUP* pSetUp)
{
    DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)miimId;
    if((pSetUp->setupFlags & DRV_MIIM_SETUP_FLAG_PREAMBLE_SUPPRESSED) != 0)
    {
        DRV_ETH_MIIMNoPreEnable(ethId);            
    }
    else
    {
        DRV_ETH_MIIMNoPreDisable(ethId);
    }

}
    
static  void _DRV_MIIM_ScanIncrement(uintptr_t miimId, const DRV_MIIM_SETUP* pSetUp)
{
    DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)miimId;
    if((pSetUp->setupFlags & DRV_MIIM_SETUP_FLAG_SCAN_ADDRESS_INCREMENT) != 0)
    {
        DRV_ETH_MIIMScanIncrEnable(ethId);
    }
    else
    {
        DRV_ETH_MIIMScanIncrDisable(ethId);
    }

}

static void _DRV_MIIM_SMIClockSet(uintptr_t miimId, uint32_t hostClock, uint32_t maxMIIMClock )
{

    DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)miimId;
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

    DRV_ETH_MIIMClockSet(ethId, ix + 1);  // program the clock

}


// ************** API implementation

// sets up the device: enable the device, sets the clock and other settings
DRV_MIIM_RESULT DRV_MIIM_DeviceSetup(uintptr_t miimId, const DRV_MIIM_SETUP* pSetUp)
{
    DRV_MIIM_RESULT res;

    // enable and make sure the MAC MII is not held in reset
    res =  _DRV_MIIM_Enable(miimId);
    
    // setup the clock
    _DRV_MIIM_SMIClockSet(miimId, pSetUp->hostClockFreq, pSetUp->maxBusFreq );

    // other settings
    _DRV_MIIM_SetupPreamble(miimId, pSetUp);
    _DRV_MIIM_ScanIncrement(miimId, pSetUp);

    return res;    
} 

// no operation for PIC32M
void DRV_MIIM_PortEnable(uintptr_t miimId)
{
}

// no operation for PIC32M
void DRV_MIIM_PortDisable(uintptr_t miimId)
{
}
    
bool DRV_MIIM_PortBusy(uintptr_t miimId)
{
    return DRV_ETH_MIIMIsBusy((DRV_ETHERNET_REGISTERS*)miimId);
}

void DRV_MIIM_WriteData(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx, uint16_t wData)
{
    DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)miimId;
    DRV_ETH_PHYAddressSet(ethId, phyAdd);
    DRV_ETH_RegisterAddressSet(ethId, regIx);
    DRV_ETH_MIIMWriteDataSet(ethId, wData);
}

void DRV_MIIM_ReadStart(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx)
{
    DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)miimId;
    DRV_ETH_PHYAddressSet(ethId, phyAdd);
    DRV_ETH_RegisterAddressSet(ethId, regIx);
    DRV_ETH_MIIMReadStart(ethId);
}

uint16_t DRV_MIIM_ReadDataGet(uintptr_t miimId)
{
    // clear the READ bit
    DRV_ETH_MIIMWriteStart((DRV_ETHERNET_REGISTERS*)miimId);
    return DRV_ETH_MIIMReadDataGet((DRV_ETHERNET_REGISTERS*)miimId);
}

// should return DRV_MIIM_TXFER_ERROR if scan not supported
DRV_MIIM_TXFER_STAT DRV_MIIM_ScanEnable(uintptr_t miimId, uint16_t phyAdd, uint16_t regIx)
{   
    DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)miimId;
    DRV_ETH_PHYAddressSet(ethId, phyAdd);
    DRV_ETH_RegisterAddressSet(ethId, regIx);
    DRV_ETH_ClearDataValid(ethId);
    DRV_ETH_MIIMScanModeEnable(ethId);
    return DRV_MIIM_TXFER_SCAN_STALE;
}
    
void DRV_MIIM_ScanDisable(uintptr_t miimId)
{ 
    DRV_ETH_MIIMScanModeDisable((DRV_ETHERNET_REGISTERS*)miimId);
}

bool DRV_MIIM_GetScanData(uintptr_t miimId, uint32_t* scanData)
{
    DRV_ETHERNET_REGISTERS* ethId = (DRV_ETHERNET_REGISTERS*)miimId;

    if(!DRV_ETH_DataNotValid(ethId))
    {   // there's data available
        if(scanData)
        {
            *scanData = DRV_MIIM_ReadDataGet(miimId);
        }
        // clear the hw data valid flag
        DRV_ETH_ClearDataValid(ethId);
        return true;
    }
    return false;
}


/*******************************************************************************
  End of File
*/

