/*******************************************************************************
  Ethernet PHY Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_dummy_thin.c

  Summary:
    Ethernet PHY Device Driver Dynamic Implementation

  Description:
    The Ethernet PHY device driver provides a simple interface to manage the
    Ethernet PHY modules on Microchip microcontrollers.

    This file implements a dummy PHY driver that does nothing but returns success to any call.
    No check is performed on the API parameters to keep the file at a minimum size.
    
    It can be used when:
        - the PHY board is externally configured and no othe PHY control/setup is needed.
            For example:
                - at power on, with strappings)
                - some other external configuration done before the TCP/IP stack takes control
        - The PHY does not use the MIIM driver and another communication mechanism is needed.
            The functions could act as a stub and completed with the reqired code could be filled in.        
        - The MAC is directly connected to another MAC and there's no need for a real PHY driver.


Notes:
    The auto negotiation is not supported and should NOT be enabled when using this dummy driver.
    The PHY should be externally correctly configured for the desired mode: duplex, speed, MDIX.

    The link status is always reported as UP.
    If the PHY becomes disconnected at run-time there will be no indication.
    The data transfers will be disrupted.


*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2024-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "system_config.h"
#include "driver/ethphy/drv_ethphy.h"
#include "driver/ethphy/drv_extphy_dummy.h"
#include "drv_extphy_dummy_priv.h"

// Local Definitions
//

// specific definition for the dummy PHY instance
typedef struct S_DRV_ETHPHY_DUMMY_INSTANCE
{
} DRV_ETHPHY_DUMMY_INSTANCE;


// the number of dummy PHY drivers
// probably just one should suffice
#define DRV_ETHPHY_DUMMY_INSTANCES    1U

static DRV_ETHPHY_DUMMY_INSTANCE              gPhyDrvInst[DRV_ETHPHY_DUMMY_INSTANCES];

// The dummy PHY driver object functions 
//
static SYS_MODULE_OBJ              DRV_ETHPHY_DummyInitialize(const SYS_MODULE_INDEX iModule, const SYS_MODULE_INIT* const init);
static void                        DRV_ETHPHY_DummyReinitialize (SYS_MODULE_OBJ object, const SYS_MODULE_INIT* const init);
static void                        DRV_ETHPHY_DummyDeinitialize (SYS_MODULE_OBJ object);
static SYS_STATUS                  DRV_ETHPHY_DummyStatus (SYS_MODULE_OBJ object);
static void                        DRV_ETHPHY_DummyTasks(SYS_MODULE_OBJ object);
static DRV_HANDLE                  DRV_ETHPHY_DummyOpen (const SYS_MODULE_INDEX iModule, const DRV_IO_INTENT ioIntent);
static void                        DRV_ETHPHY_DummyClose(DRV_HANDLE handle);
static DRV_ETHPHY_CLIENT_STATUS    DRV_ETHPHY_DummyClientStatus(DRV_HANDLE handle);
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyClientOpResult(DRV_HANDLE handle);  
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyClientOpAbort(DRV_HANDLE handle);  
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyPhyAddressGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress);
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummySetup(DRV_HANDLE  handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags);
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyRestartNegotiation(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex);
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyHWConfigFlagsGet(DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags);
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyNegotiationIsComplete(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete);
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyNegotiationResultGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult);
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyLinkStatusGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh);
static DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyReset(DRV_HANDLE handle, bool waitComplete);

// The dummy PHY driver object
//
const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_Dummy = 
{
     .phy_Initialize =             &DRV_ETHPHY_DummyInitialize,
     .phy_Reinitialize =           &DRV_ETHPHY_DummyReinitialize,
     .phy_Deinitialize =           &DRV_ETHPHY_DummyDeinitialize,
     .phy_Status =                 &DRV_ETHPHY_DummyStatus,
     .phy_Tasks =                  &DRV_ETHPHY_DummyTasks,
     .phy_Open =                   &DRV_ETHPHY_DummyOpen,
     .phy_Close =                  &DRV_ETHPHY_DummyClose,
     .phy_ClientStatus =           &DRV_ETHPHY_DummyClientStatus,
     .phy_ClientOperationResult =  &DRV_ETHPHY_DummyClientOpResult,
     .phy_ClientOperationAbort =   &DRV_ETHPHY_DummyClientOpAbort,
     .phy_PhyAddressGet =          &DRV_ETHPHY_DummyPhyAddressGet,
     .phy_Setup =                  &DRV_ETHPHY_DummySetup,
     .phy_RestartNegotiation =     &DRV_ETHPHY_DummyRestartNegotiation,
     .phy_HWConfigFlagsGet =       &DRV_ETHPHY_DummyHWConfigFlagsGet,
     .phy_NegotiationIsComplete =  &DRV_ETHPHY_DummyNegotiationIsComplete,
     .phy_NegotiationResultGet =   &DRV_ETHPHY_DummyNegotiationResultGet,
     .phy_LinkStatusGet =          &DRV_ETHPHY_DummyLinkStatusGet,
     .phy_Reset =                  &DRV_ETHPHY_DummyReset,
     .phy_VendorDataGet =          NULL,         // should never be called!
     .phy_VendorDataSet =          NULL,         // should never be called!
     .phy_VendorSMIReadStart =     NULL,         // should never be called!
     .phy_VendorSMIReadResultGet = NULL,         // should never be called!
     .phy_VendorSMIWriteStart =    NULL,         // should never be called!
     .phy_VendorSMIWriteWaitComplete = NULL,     // should never be called!
     .phy_VendorSMIOperationIsComplete = NULL,   // should never be called!
};



// just state update
static SYS_MODULE_OBJ DRV_ETHPHY_DummyInitialize(const SYS_MODULE_INDEX iModule, const SYS_MODULE_INIT* const init)
{
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst; 

    if (iModule >= DRV_ETHPHY_DUMMY_INSTANCES)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    phyInst = gPhyDrvInst + iModule;

    /* Return the driver handle */
    return (SYS_MODULE_OBJ)phyInst ;

}

// just state update
static void DRV_ETHPHY_DummyReinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT* const init)
{
}


// just state update
static void DRV_ETHPHY_DummyDeinitialize(SYS_MODULE_OBJ object)
{
} 


static SYS_STATUS DRV_ETHPHY_DummyStatus(SYS_MODULE_OBJ object)
{
    return SYS_STATUS_READY; 
} 


// no action
static void DRV_ETHPHY_DummyTasks(SYS_MODULE_OBJ object)
{
} 


// just state update
static DRV_HANDLE  DRV_ETHPHY_DummyOpen (const SYS_MODULE_INDEX iModule, const DRV_IO_INTENT ioIntent)
{
    return (DRV_HANDLE)(gPhyDrvInst + iModule);
}

// just state update
static void DRV_ETHPHY_DummyClose(DRV_HANDLE handle)
{
}

// no setup occurs
// just state update
// Note: needs to setup the pSetupFlags for the MAC!
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummySetup(DRV_HANDLE handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
{
    // basic sanity check
    if(pSetUp == NULL || pSetupFlags == NULL)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if(pSetUp->resetFunction != NULL)
    {
        (*pSetUp->resetFunction)(&DRV_ETHPHY_OBJECT_BASE_Dummy, handle);
    }

    *pSetupFlags = pSetUp->openFlags;   // pass back whatever we've received 

    return DRV_ETHPHY_RES_OK;
}


static DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_DummyClientStatus(DRV_HANDLE handle)
{
    return DRV_ETHPHY_CLIENT_STATUS_READY; 
}

// just state update
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyClientOpResult(DRV_HANDLE handle)  
{
    return DRV_ETHPHY_RES_OK;
}

// just state update
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyClientOpAbort(DRV_HANDLE handle)  
{
    return DRV_ETHPHY_RES_OK;
}

// not suported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyPhyAddressGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress)
{
    return DRV_ETHPHY_RES_OPERATION_ERR;
}

// not suported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyRestartNegotiation(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex)
{
    return DRV_ETHPHY_RES_OPERATION_ERR;
}



// not supported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyHWConfigFlagsGet(DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags)
{
    return DRV_ETHPHY_RES_OPERATION_ERR; 
}


// not supported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyNegotiationIsComplete(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete)
{
    return DRV_ETHPHY_RES_OPERATION_ERR;
}


// not supported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyNegotiationResultGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    return DRV_ETHPHY_RES_OPERATION_ERR;
}

// always up
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyLinkStatusGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh)
{
    // basic sanity check
    if(pLinkStat == NULL)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    *pLinkStat = DRV_ETHPHY_LINK_ST_UP; 
    return DRV_ETHPHY_RES_OK;
}
 
// just state update
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyReset(DRV_HANDLE handle, bool waitComplete)
{
    return DRV_ETHPHY_RES_OK;
}


