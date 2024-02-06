/*******************************************************************************
  Ethernet PHY Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_extphy_dummy.c

  Summary:
    Ethernet PHY Device Driver Dynamic Implementation

  Description:
    The Ethernet PHY device driver provides a simple interface to manage the
    Ethernet PHY modules on Microchip microcontrollers.

    This file implements a dummy PHY driver that does nothing but returns success to any call.
    However, a consistency check is performed on the API parameters that are passed in.

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
Copyright (C) 2013-2024, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

// Local Definitions
//

// specific definition for the dummy PHY instance
typedef struct _DRV_ETHPHY_DUMMY_INSTANCE
{
    uint8_t                     instanceInit;   // True if instance initialized
    uint8_t                     clientInUse;    // True if in use
    int16_t                     instanceStatus; // SYS_STATUS: Status of instance
    int16_t                     clientStatus;   // Client Status: DRV_ETHPHY_CLIENT_STATUS value
    uint16_t                    reserved;       // not used/padding

    const DRV_ETHPHY_OBJECT_BASE* pBaseObj;     // PHY base reference: this object; used for handle validation only 
} DRV_ETHPHY_DUMMY_INSTANCE;


// the number of dummy PHY drivers
// probably just one should suffice
#define DRV_ETHPHY_DUMMY_INSTANCES    1

static DRV_ETHPHY_DUMMY_INSTANCE              gPhyDrvInst[DRV_ETHPHY_DUMMY_INSTANCES];


#if (DRV_ETHPHY_DUMMY_INSTANCES == 1)
static DRV_ETHPHY_DUMMY_INSTANCE* _DRV_DUMMY_HandleToInst(uintptr_t handle)
{
    DRV_ETHPHY_DUMMY_INSTANCE* pPhyInst = (DRV_ETHPHY_DUMMY_INSTANCE*)handle;
    if(pPhyInst == gPhyDrvInst && pPhyInst->pBaseObj == &DRV_ETHPHY_OBJECT_BASE_Dummy)
    {
        return pPhyInst;
    }


    return NULL;
}
#else
// multiple instances version
// could be refined more
static DRV_ETHPHY_DUMMY_INSTANCE* _DRV_DUMMY_HandleToInst(uintptr_t handle)
{
    DRV_ETHPHY_DUMMY_INSTANCE* pPhyInst = (DRV_ETHPHY_DUMMY_INSTANCE*)handle;
    int phyIx = (int)(pPhyInst - gPhyDrvInst);
    if(phyIx >= 0 && phyIx < sizeof(gPhyDrvInst) / sizeof(*gPhyDrvInst))
    {
        if(pPhyInst == gPhyDrvInst + phyIx)
        {
            if(pPhyInst->pBaseObj == &DRV_ETHPHY_OBJECT_BASE_Dummy)
            {
                return pPhyInst;
            }
        }
    }

    return 0;
}
#endif  // (DRV_ETHPHY_DUMMY_INSTANCES == 1)

// The dummy PHY driver object functions 
//
SYS_MODULE_OBJ              DRV_ETHPHY_DummyInitialize (const SYS_MODULE_INDEX index, const SYS_MODULE_INIT* const init);
void                        DRV_ETHPHY_DummyReinitialize (SYS_MODULE_OBJ object, const SYS_MODULE_INIT* const init);
void                        DRV_ETHPHY_DummyDeinitialize (SYS_MODULE_OBJ object);
SYS_STATUS                  DRV_ETHPHY_DummyStatus (SYS_MODULE_OBJ object);
void                        DRV_ETHPHY_DummyTasks(SYS_MODULE_OBJ object);
DRV_HANDLE                  DRV_ETHPHY_DummyOpen(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT intent);
void                        DRV_ETHPHY_DummyClose(DRV_HANDLE handle);
DRV_ETHPHY_CLIENT_STATUS    DRV_ETHPHY_DummyClientStatus(DRV_HANDLE handle);
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyClientOperationResult(DRV_HANDLE handle);  
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyClientOperationAbort(DRV_HANDLE handle);  
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyPhyAddressGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress);
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummySetup(DRV_HANDLE  handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags);
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyRestartNegotiation(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex);
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyHWConfigFlagsGet(DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags);
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyNegotiationIsComplete(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete);
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyNegotiationResultGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult);
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyLinkStatusGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh);
DRV_ETHPHY_RESULT           DRV_ETHPHY_DummyReset(DRV_HANDLE handle, bool waitComplete);

// The dummy PHY driver object
//
const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_Dummy = 
{
     .DRV_ETHPHY_Initialize =             DRV_ETHPHY_DummyInitialize,
     .DRV_ETHPHY_Reinitialize =           DRV_ETHPHY_DummyReinitialize,
     .DRV_ETHPHY_Deinitialize =           DRV_ETHPHY_DummyDeinitialize,
     .DRV_ETHPHY_Status =                 DRV_ETHPHY_DummyStatus,
     .DRV_ETHPHY_Tasks =                  DRV_ETHPHY_DummyTasks,
     .DRV_ETHPHY_Open =                   DRV_ETHPHY_DummyOpen,
     .DRV_ETHPHY_Close =                  DRV_ETHPHY_DummyClose,
     .DRV_ETHPHY_ClientStatus =           DRV_ETHPHY_DummyClientStatus,
     .DRV_ETHPHY_ClientOperationResult =  DRV_ETHPHY_DummyClientOperationResult,
     .DRV_ETHPHY_ClientOperationAbort =   DRV_ETHPHY_DummyClientOperationAbort,
     .DRV_ETHPHY_PhyAddressGet =          DRV_ETHPHY_DummyPhyAddressGet,
     .DRV_ETHPHY_Setup =                  DRV_ETHPHY_DummySetup,
     .DRV_ETHPHY_RestartNegotiation =     DRV_ETHPHY_DummyRestartNegotiation,
     .DRV_ETHPHY_HWConfigFlagsGet =       DRV_ETHPHY_DummyHWConfigFlagsGet,
     .DRV_ETHPHY_NegotiationIsComplete =  DRV_ETHPHY_DummyNegotiationIsComplete,
     .DRV_ETHPHY_NegotiationResultGet =   DRV_ETHPHY_DummyNegotiationResultGet,
     .DRV_ETHPHY_LinkStatusGet =          DRV_ETHPHY_DummyLinkStatusGet,
     .DRV_ETHPHY_Reset =                  DRV_ETHPHY_DummyReset,
     .DRV_ETHPHY_VendorDataGet =          NULL,         // should never be called!
     .DRV_ETHPHY_VendorDataSet =          NULL,         // should never be called!
     .DRV_ETHPHY_VendorSMIReadStart =     NULL,         // should never be called!
     .DRV_ETHPHY_VendorSMIReadResultGet = NULL,         // should never be called!
     .DRV_ETHPHY_VendorSMIWriteStart =    NULL,         // should never be called!
     .DRV_ETHPHY_VendorSMIWriteWaitComplete = NULL,     // should never be called!
     .DRV_ETHPHY_VendorSMIOperationIsComplete = NULL,   // should never be called!
};



// just state update
SYS_MODULE_OBJ DRV_ETHPHY_DummyInitialize(const SYS_MODULE_INDEX iModule, const SYS_MODULE_INIT* const init)
{
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst; 

    if (iModule >= DRV_ETHPHY_DUMMY_INSTANCES)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    phyInst = gPhyDrvInst + iModule;
    if(phyInst->instanceInit == true)
    {   // already initalized
        return (SYS_MODULE_OBJ)phyInst ;
    }

    phyInst->instanceInit = true;      // Set object to be in use
    phyInst->instanceStatus = SYS_STATUS_READY; // Set instance state
    phyInst->pBaseObj = &DRV_ETHPHY_OBJECT_BASE_Dummy;
    /* Return the driver handle */
    return (SYS_MODULE_OBJ)phyInst ;

}

// just state update
void DRV_ETHPHY_DummyReinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT* const init)
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(object);
    if(phyInst != 0)
    {
        phyInst->instanceStatus = SYS_STATUS_READY; // Set instance state
    }
}


// just state update
void DRV_ETHPHY_DummyDeinitialize(SYS_MODULE_OBJ object)
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(object);
    if(phyInst != 0)
    {
        /* Set the Device Status */
        phyInst->instanceStatus  = SYS_STATUS_UNINITIALIZED;

        /* Remove the driver usage */
        phyInst->instanceInit  = false;
    }
} 


SYS_STATUS DRV_ETHPHY_DummyStatus(SYS_MODULE_OBJ object)
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(object);
    if(phyInst != 0)
    {
        /* Return the status associated with the driver handle */
        return phyInst->instanceStatus;
    }

    return SYS_STATUS_ERROR;
} 


// no action
void DRV_ETHPHY_DummyTasks(SYS_MODULE_OBJ object)
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(object);
    if(phyInst != 0)
    {
    }
} 


// just state update
DRV_HANDLE  DRV_ETHPHY_DummyOpen (const SYS_MODULE_INDEX iModule, const DRV_IO_INTENT ioIntent)
{
    /* Multi client variables are removed from single client builds. */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst;

    /* Validate the driver index */
    /* If there is anything specific to the module & needs to be checked, should
       be handled in this section with an || condition.
       May be something like ioIntent test for Exclusive access */
    if (iModule >= DRV_ETHPHY_DUMMY_INSTANCES)
    {
        return DRV_HANDLE_INVALID;
    }

    phyInst = gPhyDrvInst + iModule; 
    if(phyInst->instanceInit == false)
    {   // not opened
        return DRV_HANDLE_INVALID;
    }

    /* Setup client operations */

    // Allocate the client object and set the flag as in use
    // there's only one client: the MAC
    if(phyInst->clientInUse != 0)
    {   // already in use
        return DRV_HANDLE_INVALID;
    }

    phyInst->clientInUse    = true;

    /* Update the Client Status */
    phyInst->clientStatus = DRV_ETHPHY_CLIENT_STATUS_READY;

    /* Return the client object */
    return ((DRV_HANDLE)phyInst);
}

// just state update
void DRV_ETHPHY_DummyClose(DRV_HANDLE handle)
{
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            /* Free the Client Instance */
            phyInst->clientInUse = false ;

            /* Update the Client Status */
            phyInst->clientStatus = DRV_ETHPHY_CLIENT_STATUS_CLOSED;
        }
    }
}

// no setup occurs
// just state update
// Note: needs to setup the pSetupFlags for the MAC!
DRV_ETHPHY_RESULT DRV_ETHPHY_DummySetup(DRV_HANDLE handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
{

    /* Check for the Client validity */
    if(handle == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
    if(phyInst == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(phyInst->clientStatus != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // not ready or another op going on; 
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

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


DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_DummyClientStatus(DRV_HANDLE handle)
{
    /* Check for the Client validity */
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            /* Return the client status associated with the handle passed */
            return phyInst->clientStatus;
        }
    }

    return DRV_ETHPHY_CLIENT_STATUS_ERROR;
}

// just state update
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyClientOperationResult(DRV_HANDLE handle)  
{
    /* Check for the Client validity */
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// just state update
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyClientOperationAbort(DRV_HANDLE handle)  
{
    /* Check for the Client validity */
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// not suported
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyPhyAddressGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress)
{
    (void)portIndex;
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}



// not suported
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyRestartNegotiation(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex)
{
    (void)portIndex;

    /* Check for the Client validity */
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}



// not supported
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyHWConfigFlagsGet(DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags)
{
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR; 
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


// not supported
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyNegotiationIsComplete(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete)
{
    (void)portIndex;

    /* Check for the Client validity */
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


// not supported
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyNegotiationResultGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    (void)portIndex;

    /* Check for the Client validity */
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// always up
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyLinkStatusGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh)
{
    (void)portIndex;

    /* Check for the Client validity */
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {
            if(phyInst->clientStatus != DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            // basic sanity check
            if(pLinkStat == 0)
            {
                return DRV_ETHPHY_RES_OPERATION_ERR;
            }
            
            *pLinkStat = DRV_ETHPHY_LINK_ST_UP; 
            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}
 
// just state update
DRV_ETHPHY_RESULT DRV_ETHPHY_DummyReset(DRV_HANDLE handle, bool waitComplete)
{

    /* Check for the Client validity */
    if(handle != 0)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = _DRV_DUMMY_HandleToInst(handle);
        if(phyInst != 0)
        {

            if(phyInst->clientStatus != DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


