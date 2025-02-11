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
    uint8_t                     instanceInit;   // True if instance initialized
    uint8_t                     clientInUse;    // True if in use
    int16_t                     instanceStatus; // SYS_STATUS: Status of instance
    int16_t                     clientStatus;   // Client Status: DRV_ETHPHY_CLIENT_STATUS value
    uint16_t                    reserved;       // not used/padding

    const DRV_ETHPHY_OBJECT_BASE* pBaseObj;     // PHY base reference: this object; used for handle validation only 
} DRV_ETHPHY_DUMMY_INSTANCE;


// the number of dummy PHY drivers
// probably just one should suffice
#define DRV_ETHPHY_DUMMY_INSTANCES    1U

static DRV_ETHPHY_DUMMY_INSTANCE              gPhyDrvInst[DRV_ETHPHY_DUMMY_INSTANCES];


#if (DRV_ETHPHY_DUMMY_INSTANCES == 1U)
static DRV_ETHPHY_DUMMY_INSTANCE* F_DRV_DUMMY_HandleToInst(uintptr_t handle)
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
static DRV_ETHPHY_DUMMY_INSTANCE* F_DRV_DUMMY_HandleToInst(uintptr_t handle)
{
    DRV_ETHPHY_DUMMY_INSTANCE* pPhyInst = (DRV_ETHPHY_DUMMY_INSTANCE*)handle;
    int phyIx = pPhyInst - gPhyDrvInst;
    if(phyIx >= 0 && phyIx < (int)sizeof(gPhyDrvInst) / (int)sizeof(*gPhyDrvInst))
    {
        if(pPhyInst == gPhyDrvInst + phyIx)
        {
            if(pPhyInst->pBaseObj == &DRV_ETHPHY_OBJECT_BASE_Dummy)
            {
                return pPhyInst;
            }
        }
    }

    return NULL;
}
#endif  // (DRV_ETHPHY_DUMMY_INSTANCES == 1U)

// The dummy PHY driver object functions 
//
static SYS_MODULE_OBJ              DRV_ETHPHY_DummyInitialize(const SYS_MODULE_INDEX iModule, const SYS_MODULE_INIT* const init);
static void                        DRV_ETHPHY_DummyReinitialize (SYS_MODULE_OBJ object, const SYS_MODULE_INIT* const init);
static void                        DRV_ETHPHY_DummyDeinitialize (SYS_MODULE_OBJ object);
static SYS_STATUS                  DRV_ETHPHY_DummyStatus (SYS_MODULE_OBJ object);
static void                        DRV_ETHPHY_DummyTasks(SYS_MODULE_OBJ object);
static DRV_HANDLE                  DRV_ETHPHY_DummyOpen(const SYS_MODULE_INDEX iModule, const DRV_IO_INTENT ioIntent);
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
    if(phyInst->instanceInit == 1U)
    {   // already initalized
        return (SYS_MODULE_OBJ)phyInst ;
    }

    phyInst->instanceInit = 1U;      // Set object to be in use
    phyInst->instanceStatus = (int16_t)SYS_STATUS_READY; // Set instance state
    phyInst->pBaseObj = &DRV_ETHPHY_OBJECT_BASE_Dummy;
    /* Return the driver handle */
    return (SYS_MODULE_OBJ)phyInst ;

}

// just state update
static void DRV_ETHPHY_DummyReinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT* const init)
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(object);
    if(phyInst != NULL)
    {
        phyInst->instanceStatus = (int16_t)SYS_STATUS_READY; // Set instance state
    }
}


// just state update
static void DRV_ETHPHY_DummyDeinitialize(SYS_MODULE_OBJ object)
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(object);
    if(phyInst != NULL)
    {
        /* Set the Device Status */
        phyInst->instanceStatus  = (int16_t)SYS_STATUS_UNINITIALIZED;

        /* Remove the driver usage */
        phyInst->instanceInit  = 0U;
    }
} 


static SYS_STATUS DRV_ETHPHY_DummyStatus(SYS_MODULE_OBJ object)
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(object);
    if(phyInst != NULL)
    {
        /* Return the status associated with the driver handle */
        return (SYS_STATUS)phyInst->instanceStatus;
    }

    return SYS_STATUS_ERROR;
} 


// no action
static void DRV_ETHPHY_DummyTasks(SYS_MODULE_OBJ object)
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(object);
    if(phyInst != NULL)
    {
    }
} 


// just state update
static DRV_HANDLE  DRV_ETHPHY_DummyOpen (const SYS_MODULE_INDEX iModule, const DRV_IO_INTENT ioIntent)
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
    if(phyInst->instanceInit == 0U)
    {   // not opened
        return DRV_HANDLE_INVALID;
    }

    /* Setup client operations */

    // Allocate the client object and set the flag as in use
    // there's only one client: the MAC
    if(phyInst->clientInUse != 0U)
    {   // already in use
        return DRV_HANDLE_INVALID;
    }

    phyInst->clientInUse = 1U;

    /* Update the Client Status */
    phyInst->clientStatus = (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY;

    /* Return the client object */
    return ((DRV_HANDLE)phyInst);
}

// just state update
static void DRV_ETHPHY_DummyClose(DRV_HANDLE handle)
{
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            /* Free the Client Instance */
            phyInst->clientInUse = 0U ;

            /* Update the Client Status */
            phyInst->clientStatus = (int16_t)DRV_ETHPHY_CLIENT_STATUS_CLOSED;
        }
    }
}

// no setup occurs
// just state update
// Note: needs to setup the pSetupFlags for the MAC!
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummySetup(DRV_HANDLE handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
{

    /* Check for the Client validity */
    if(handle == 0U)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
    if(phyInst == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(phyInst->clientStatus != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
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


static DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_DummyClientStatus(DRV_HANDLE handle)
{
    /* Check for the Client validity */
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            /* Return the client status associated with the handle passed */
            return (DRV_ETHPHY_CLIENT_STATUS)phyInst->clientStatus;
        }
    }

    return DRV_ETHPHY_CLIENT_STATUS_ERROR;
}

// just state update
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyClientOpResult(DRV_HANDLE handle)  
{
    /* Check for the Client validity */
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// just state update
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyClientOpAbort(DRV_HANDLE handle)  
{
    /* Check for the Client validity */
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// not suported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyPhyAddressGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress)
{
    (void)portIndex;
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}



// not suported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyRestartNegotiation(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex)
{
    (void)portIndex;

    /* Check for the Client validity */
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}



// not supported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyHWConfigFlagsGet(DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags)
{
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR; 
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


// not supported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyNegotiationIsComplete(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete)
{
    (void)portIndex;

    /* Check for the Client validity */
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


// not supported
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyNegotiationResultGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    (void)portIndex;

    /* Check for the Client validity */
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            return DRV_ETHPHY_RES_OPERATION_ERR;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// always up
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyLinkStatusGet(DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh)
{
    (void)portIndex;

    /* Check for the Client validity */
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {
            if(phyInst->clientStatus != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            // basic sanity check
            if(pLinkStat == NULL)
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
static DRV_ETHPHY_RESULT DRV_ETHPHY_DummyReset(DRV_HANDLE handle, bool waitComplete)
{

    /* Check for the Client validity */
    if(handle != 0U)
    {
        DRV_ETHPHY_DUMMY_INSTANCE * phyInst = F_DRV_DUMMY_HandleToInst(handle);
        if(phyInst != NULL)
        {

            if(phyInst->clientStatus != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


