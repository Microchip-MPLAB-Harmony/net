/*******************************************************************************
  ENCx24J600 Driver Public Interface
  Company:
    Microchip Technology Inc.

  File Name:
    drv_encx24j600_api.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2014-2020 Microchip Technology Inc. and its subsidiaries.

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

#include "drv_encx24j600_local.h"
#include "bus/spi/drv_encx24j600_spi_bus.h"
#include "drv_encx24j600_utils.h"
#include "driver/encx24j600/drv_encx24j600.h"

#define TCPIP_THIS_MODULE_ID   TCPIP_MODULE_MAC_ENCJ600
// Data needed by the TCPIP Stack
const TCPIP_MAC_OBJECT DRV_ENCX24J600_MACObject =
{
    .macId = TCPIP_MODULE_MAC_ENCJ600,
    .macType = TCPIP_MAC_TYPE_ETH,    
    .macName = "ENCX24J600",
    .TCPIP_MAC_Initialize = DRV_ENCX24J600_StackInitialize,
#if (TCPIP_STACK_MAC_DOWN_OPERATION != 0)
    .TCPIP_MAC_Deinitialize = DRV_ENCX24J600_Deinitialize,
    .TCPIP_MAC_Reinitialize = DRV_ENCX24J600_Reinitialize,
#else
    .TCPIP_MAC_Deinitialize = 0,
    .TCPIP_MAC_Reinitialize = 0,
#endif  // (TCPIP_STACK_DOWN_OPERATION != 0)
    .TCPIP_MAC_Status = DRV_ENCX24J600_Status,
    .TCPIP_MAC_Tasks = DRV_ENCX24J600_Tasks,
    .TCPIP_MAC_Open = DRV_ENCX24J600_Open,
    .TCPIP_MAC_Close = DRV_ENCX24J600_Close,
    .TCPIP_MAC_LinkCheck = DRV_ENCX24J600_LinkCheck,
    .TCPIP_MAC_RxFilterHashTableEntrySet = DRV_ENCX24J600_RxFilterHashTableEntrySet,
    .TCPIP_MAC_PowerMode = DRV_ENCX24J600_PowerMode,
    .TCPIP_MAC_PacketTx = DRV_ENCX24J600_PacketTx,
    .TCPIP_MAC_PacketRx = DRV_ENCX24J600_PacketRx,
    .TCPIP_MAC_Process = DRV_ENCX24J600_Process,
    .TCPIP_MAC_StatisticsGet = DRV_ENCX24J600_StatisticsGet,
    .TCPIP_MAC_ParametersGet = DRV_ENCX24J600_ParametersGet,
    .TCPIP_MAC_RegisterStatisticsGet = DRV_ENCX24J600_RegisterStatisticsGet,
    .TCPIP_MAC_ConfigGet = DRV_ENCX24J600_ConfigGet,
    .TCPIP_MAC_EventMaskSet = DRV_ENCX24J600_EventMaskSet,
    .TCPIP_MAC_EventAcknowledge = DRV_ENCX24J600_EventAcknowledge,
    .TCPIP_MAC_EventPendingGet = DRV_ENCX24J600_EventPendingGet,
};

// Local information
static DRV_ENCX24J600_DriverInfo drvEncX24J600DrvInst[DRV_ENCX24J600_INSTANCES_NUMBER] __attribute__((aligned(4)));
static DRV_ENCX24J600_ClientInfo drvEncX24J600ClntInst[DRV_ENCX24J600_CLIENT_INSTANCES_IDX0];
static uint8_t drvEncX24J600NumOfDrivers = 0;
static OSAL_MUTEX_HANDLE_TYPE  drvEncX24J600ClntMutex;

bool _DRV_ENCX24J600_ValidateDriverInstance(DRV_ENCX24J600_DriverInfo * ptr)
{
    uint8_t x;
    for (x = 0; x < DRV_ENCX24J600_INSTANCES_NUMBER; x++)
    {
        if (ptr == &(drvEncX24J600DrvInst[x]))
        {
            if (ptr->inUse)
            {
                return true;
            }
            return false;
        }
    }
    return false;
}

DRV_ENCX24J600_DriverInfo * _DRV_ENCX24J600_ValidateClientHandle(DRV_ENCX24J600_ClientInfo * ptr)
{
    uint8_t x;
    for (x = 0; x < DRV_ENCX24J600_CLIENT_INSTANCES_IDX0; x++)
    {
        if (ptr == &(drvEncX24J600ClntInst[x]))
        {
            if (!ptr->inUse)
            {
                return NULL;
            }
            if (_DRV_ENCX24J600_ValidateDriverInstance(ptr->pDrvInst))
            {
                return ptr->pDrvInst;
            }
            return NULL;
        }
    }
    return NULL;
}


// *****************************************************************************
/* ENCX24J600 Initialization

    Summary:
    Initializes the ENCx24J600 Driver Instance, with the configuration data.

    Details:
    This function initializes the ENCx24J600 Driver with configuration data
    passed into it by either the system_init() function or by the
    DRV_ENCX24J600_StackInitialize() function.  Calling this function alone is
    not enough to initialize the driver,  DRV_ENCX24J600_SetMacCtrlInfo() must
    be called with valid data before the driver is ready to be opened.

    Preconditions
    None

    Parameters
        index: This is the index of the driver instance to be initialized.  The definition DRV_ENCX24J600_NUM_DRV_INSTANCES controls how many instances are available.
        init: This is a pointer to a DRV_ENX24J600_CONFIG structure.

    Returns
        If successful: returns a valid handle to the driver instance
        If unsuccessful: returns SYS_MODULE_OBJ_INVALID
*/
SYS_MODULE_OBJ DRV_ENCX24J600_Initialize(SYS_MODULE_INDEX index, SYS_MODULE_INIT * init)
{
    if (index >= DRV_ENCX24J600_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    if (init == NULL)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    DRV_ENCX24J600_DriverInfo * pDrvInst = &(drvEncX24J600DrvInst[index]);
    if (pDrvInst->inUse)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    // Clear out any cruft that might be in there
    memset(pDrvInst, 0, sizeof(*pDrvInst));

    pDrvInst->inUse = true;
    OSAL_RESULT res = 0;
    res = res; // Force ignore of unused variable, which will be used if SYS_ASSERT was used
    if (drvEncX24J600NumOfDrivers == 0)
    {
        res = OSAL_MUTEX_Create(&drvEncX24J600ClntMutex);
        SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not create the OSAL Mutex for client protection");
    }
    drvEncX24J600NumOfDrivers++;

    res = OSAL_MUTEX_Create(&(pDrvInst->drvMutex));
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not create the OSAL Mutex for driver protection");

    memcpy(&(pDrvInst->drvCfg), init, sizeof(DRV_ENCX24J600_Configuration));

    TCPIP_Helper_ProtectedSingleListInitialize(&pDrvInst->rxFreePackets);
    TCPIP_Helper_ProtectedSingleListInitialize(&pDrvInst->rxWaitingForPickupPackets);
    TCPIP_Helper_ProtectedSingleListInitialize(&pDrvInst->txPendingPackets);

    return (SYS_MODULE_OBJ)pDrvInst;
}

// *****************************************************************************
/* ENCX24J600 Deinitialization
    Summary:
    Deinitializes the ENCx24J600 Driver Instance.

    Details:
    This function deallocates any resources allocated by the initialization function.

    Preconditions:
    The driver had to be successfully initialized with DRV_ENCX24J600_Initialize().

    Parameters:
        Object: the valid object returned from DRV_ENCX24J600_Initialize

    Returns:
        None
*/
void DRV_ENCX24J600_Deinitialize(SYS_MODULE_OBJ object)
{
    DRV_ENCX24J600_DriverInfo * pDrvInst = ( DRV_ENCX24J600_DriverInfo *)object;    
    TCPIP_MAC_PACKET* pkt;
	
    if (!_DRV_ENCX24J600_ValidateDriverInstance(pDrvInst))
    {
        return;
    }
    
    
    DRV_ENCX24J600_SPI_DeinitializeInterface(pDrvInst);

    OSAL_MUTEX_Delete(&pDrvInst->drvMutex);
    // Clear out any cruft that might be in there

    while (TCPIP_Helper_ProtectedSingleListCount(&pDrvInst->rxFreePackets) != 0)
    {
        TCPIP_MAC_PACKET * pkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_ProtectedSingleListHeadRemove(&pDrvInst->rxFreePackets);
        (*pDrvInst->stackCfg.pktFreeF)(pkt);
    }
    TCPIP_Helper_ProtectedSingleListDeinitialize(&pDrvInst->rxFreePackets);
    
    while ((pkt = (TCPIP_MAC_PACKET*)TCPIP_Helper_ProtectedSingleListHeadRemove(&pDrvInst->txPendingPackets)) != 0)
    {
        pkt->pktFlags &= ~TCPIP_MAC_PKT_FLAG_QUEUED;        
        // acknowledge the packet
        (*pDrvInst->stackCfg.pktAckF)(pkt, TCPIP_MAC_PKT_ACK_NET_DOWN, TCPIP_THIS_MODULE_ID);
       
    } 
    TCPIP_Helper_ProtectedSingleListDeinitialize(&pDrvInst->txPendingPackets);
    TCPIP_Helper_ProtectedSingleListDeinitialize(&pDrvInst->rxWaitingForPickupPackets);

    memset(pDrvInst, 0, sizeof(*pDrvInst));
    drvEncX24J600NumOfDrivers--;
    if (drvEncX24J600NumOfDrivers == 0)
    {
        OSAL_MUTEX_Delete(&drvEncX24J600ClntMutex);
    }
}

// *****************************************************************************
/* ENCX24J600 Reinitialization
    Summary:
    Reinitializes the instance of the ENCX24J600 driver

    Details:
    This function will deinitialize and initialize the driver instance.  As with
    DRV_ENCX24J600_Initialize DRV_ENCX24J600_SetMacCtrlInfo() must be called for
    the driver to be useful.
    Note: this function isnâ??t planned to be implemented for the first release.

    Preconditions:
    The driver had to be successfully initialized with DRV_ENCX24J600_Initialize().

    Parameters:
        object: The object valid passed back to DRV_ENCX24J600_Initialize()
        init: The new initialization structure.

    Returns:
        None
    */
void DRV_ENCX24J600_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT * const init)
{

}

// *****************************************************************************
/* ENCX24J600 Status

    Summary:
    Gets the current status of the driver

    Details:
    This function will get the status of the driver instance.

    Preconditions:
    The driver had to be successfully initialized with DRV_ENCX24J600_Initialize().

    Parameters:
        object: The object valid passed back to DRV_ENCX24J600_Initialize()

    Returns:
        SYS_STATUS_ERROR: if an invalid handle has been passed in
        SYS_STATUS_UNINITIALIZED: if the driver hasnâ??t completed initialization
        SYS_STATUS_BUSY: if the driver is closing and moving to the closed state
        SYS_STATUS_READY: if the driver is ready for client commands
*/
SYS_STATUS DRV_ENCX24J600_Status(SYS_MODULE_OBJ object)
{
    DRV_ENCX24J600_DriverInfo * pDrvInst = ( DRV_ENCX24J600_DriverInfo *)object;
    if (!_DRV_ENCX24J600_ValidateDriverInstance(pDrvInst))
    {
        return SYS_STATUS_ERROR;
    }
    switch(pDrvInst->mainStateInfo.state)
    {
        case DRV_ENCX24J600_MS_UNINITIALIZED:
        case DRV_ENCX24J600_MS_INITIALIZATION:
            return SYS_STATUS_UNINITIALIZED;
        case DRV_ENCX24J600_MS_CLOSED:
        case DRV_ENCX24J600_MS_RUNNING:
            return SYS_STATUS_READY;
        case DRV_ENCX24J600_MS_CLOSING:
            return SYS_STATUS_BUSY;
        default:
            return SYS_STATUS_ERROR;
    }
}

// *****************************************************************************
/* ENCX24J600 Tasks

    Summary:
    Main task function for the driver

    Details:
    This function will execute the main state machine for the ENCX24J600 driver.

    Preconditions:
    The driver had to be successfully initialized with DRV_ENCX24J600_Initialize().

    Parameters:
        object: The object valid passed back to DRV_ENCX24J600_Initialize()

    Returns:
        None
*/
void DRV_ENCX24J600_Tasks(SYS_MODULE_OBJ object)
{
    DRV_ENCX24J600_DriverInfo * pDrvInst = ( DRV_ENCX24J600_DriverInfo *)object;
    if (!_DRV_ENCX24J600_ValidateDriverInstance(pDrvInst))
    {
        return;
    }
    DRV_ENCX24J600_MainStateTask(pDrvInst);
}

// *****************************************************************************
/* ENCX24J600 Set MAC Control Information

    Summary:
    This function sets the MAC control information for the driver.

    Details:
    This function is used to pass in the TCPIP_MAC_CONTROL_INIT information that
    is used for allocation and deallocation of memory, event signaling, etc.
    This function is needed to be called so that the driver can enter
    initialization state when the tasks function is called.

    Preconditions:
    The driver had to be successfully initialized with DRV_ENCX24J600_Initialize().

    Parameters:
        object: The object valid passed back to DRV_ENCX24J600_Initialize()
        init: The structure containing the MAC control information.

    Returns:
        None
*/
void DRV_ENCX24J600_SetMacCtrlInfo(SYS_MODULE_OBJ object, TCPIP_MAC_MODULE_CTRL * init)
{
    DRV_ENCX24J600_DriverInfo * pDrvInst = ( DRV_ENCX24J600_DriverInfo *)object;
    if (!_DRV_ENCX24J600_ValidateDriverInstance(pDrvInst))
    {
        return;
    }
    memcpy(&(pDrvInst->stackCfg), init, sizeof(TCPIP_MAC_MODULE_CTRL));
    pDrvInst->stackCfgReady = true;
    int32_t res = DRV_ENCX24J600_SPI_InitializeInterface(pDrvInst);
    res = res;// Silly compiler warning fix
    SYS_ASSERT(res == 0, "Could not initialize the bus interface");
    pDrvInst->mainStateInfo.state = DRV_ENCX24J600_MS_INITIALIZATION;
    uint8_t count;
    for (count = 0; count < pDrvInst->drvCfg.rxDescriptors; count++)
    {
        TCPIP_MAC_PACKET * pkt = (*pDrvInst->stackCfg.pktAllocF)(pDrvInst->drvCfg.rxDescBufferSize, pDrvInst->drvCfg.rxDescBufferSize, TCPIP_MAC_PKT_FLAG_STATIC);
        pkt->ackFunc = DRV_ENCX24J600_RxPacketAck;
        pkt->ackParam = pDrvInst;
        TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->rxFreePackets, (SGL_LIST_NODE*)pkt);

    }
}

// *****************************************************************************
/* ENCX24J600 Stack Initialization

    Summary:
    This function initializes the driver with a TCPIP_MAC_INIT object

    Details:
    This function is used by the TCP/IP stack to fully initialize the driver with
    both the ENCX24J600 specific configuration and the MAC control information.
    With this function there is no need to call DRV_ENCX24J600_SetMacCtrlInfo()

    Preconditions:
    None

    Parameters:
        index: This is the index of the driver instance to be initialized.  The definition DRV_ENCX24J600_NUM_DRV_INSTANCES controls how many instances are available.
        init: This is a pointer to a TCPIP_MAC_INIT structure.

    Returns:
        If successful: returns a valid handle to the driver instance
        If unsuccessful: returns SYS_MODULE_OBJ_INVALID
*/
SYS_MODULE_OBJ DRV_ENCX24J600_StackInitialize(SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    const TCPIP_MAC_INIT * const ptr = (const TCPIP_MAC_INIT * const)init;
    SYS_MODULE_OBJ pDrvInst = DRV_ENCX24J600_Initialize(index - TCPIP_MODULE_MAC_ENCJ600, (SYS_MODULE_INIT *)ptr->moduleData);
    if (pDrvInst == SYS_MODULE_OBJ_INVALID)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    DRV_ENCX24J600_SetMacCtrlInfo(pDrvInst, (TCPIP_MAC_MODULE_CTRL *)ptr->macControl);

    return pDrvInst;
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* ENCX24J600 Open
    Summary:
    This function is called by the client to open a handle to a driver instance

    Details:
    The client will call this function to open a handle to the driver.  When the
    first instance is opened than the driver will send the RX enabled command to
    the ENC hardware.

    Preconditions:
    The driver had to be successfully initialized with DRV_ENCX24J600_Initialize().

    Parameters:
        index: This is the index of the driver instance to be initialized.  The
                definition DRV_ENCX24J600_NUM_DRV_INSTANCES controls how many
                instances are available.
        intent: the intent to use when opening the driver.  Only exclusive is supported

    Returns:
        If successful: returns a valid handle.
        If unsuccessful: returns INVALID_HANDLE
*/
DRV_HANDLE DRV_ENCX24J600_Open(SYS_MODULE_INDEX index, DRV_IO_INTENT intent)
{
    uint8_t x;
    DRV_ENCX24J600_DriverInfo * pDrvInst = &(drvEncX24J600DrvInst[index - TCPIP_MODULE_MAC_ENCJ600]);
    DRV_ENCX24J600_ClientInfo * ptr = NULL;
    if (!_DRV_ENCX24J600_ValidateDriverInstance(pDrvInst))
    {
        return DRV_HANDLE_INVALID;
    }
    if (!pDrvInst->stackCfgReady)
    {
        return DRV_HANDLE_INVALID;
    }
    OSAL_RESULT res = OSAL_MUTEX_Lock(&drvEncX24J600ClntMutex, OSAL_WAIT_FOREVER);
    res = res; // Force ignore of unused variable, which will be used if SYS_ASSERT was used
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not get client protection mutex");
    res = OSAL_MUTEX_Lock(&pDrvInst->drvMutex, OSAL_WAIT_FOREVER);
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not get driver instance mutex");
    if ((intent&DRV_IO_INTENT_EXCLUSIVE) == DRV_IO_INTENT_EXCLUSIVE)
    {
        if (pDrvInst->numClients != 0)
        {
            res = OSAL_MUTEX_Unlock(&drvEncX24J600ClntMutex);
            SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release client protection mutex");
            res = OSAL_MUTEX_Unlock(&pDrvInst->drvMutex);
            SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release driver instance mutex");
            return DRV_HANDLE_INVALID;
        }
    }
    else
    {
        if (pDrvInst->exclusiveMode)
        {
            res = OSAL_MUTEX_Unlock(&drvEncX24J600ClntMutex);
            SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release client protection mutex");
            res = OSAL_MUTEX_Unlock(&pDrvInst->drvMutex);
            SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release driver instance mutex");
            return DRV_HANDLE_INVALID;
        }
    }

    for (x = 0; x < DRV_ENCX24J600_CLIENT_INSTANCES_IDX0; x++)
    {
        ptr = &drvEncX24J600ClntInst[x];
        if (ptr->inUse)
        {
            continue;
        }
        else
        {
            ptr->inUse = true;
            ptr->pDrvInst = pDrvInst;
            pDrvInst->numClients++;
            if ((intent&DRV_IO_INTENT_EXCLUSIVE) == DRV_IO_INTENT_EXCLUSIVE)
            {
                pDrvInst->exclusiveMode = true;
            }
            res = OSAL_MUTEX_Unlock(&drvEncX24J600ClntMutex);
            SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release client protection mutex");
            res = OSAL_MUTEX_Unlock(&pDrvInst->drvMutex);
            SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release driver instance mutex");
            return (DRV_HANDLE)ptr;
        }
    }
    res = OSAL_MUTEX_Unlock(&drvEncX24J600ClntMutex);
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release client protection mutex");
    res = OSAL_MUTEX_Unlock(&pDrvInst->drvMutex);
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release driver instance mutex");
    return DRV_HANDLE_INVALID;
}


// *****************************************************************************
/* ENCX24J600 Close

    Summary:
    Closes a client handle to the driver

    Details:
    This function closes a handle to the driver.  If itâ??s the last client open
    the driver will send an RX Disable command to the ENC hardware and move to t
    he closed state.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        Handle: the successfully opened handle

    Returns:
        None
*/
void DRV_ENCX24J600_Close(DRV_HANDLE handle)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)handle;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return;
    }
    OSAL_RESULT res = OSAL_MUTEX_Lock(&pDrvInst->drvMutex, OSAL_WAIT_FOREVER);
    res = res; // Force ignore of unused variable, which will be used if SYS_ASSERT was used

    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not get driver lock");
    (*pDrvInst->busVTable->fpCloseIf)(pDrvInst);
    pDrvInst->numClients --;
    pDrvInst->exclusiveMode = false;
    res = OSAL_MUTEX_Unlock(&pDrvInst->drvMutex);
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not release driver instance mutex");
    memset(pClient, 0, sizeof(DRV_ENCX24J600_ClientInfo));
}

// *****************************************************************************
/* ENCX24J600  Link Check

    Summary:
    This function returns the status of the link

    Details:
    This function checks the status of the link and returns it to the caller.

    Preconditions
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle

    Returns:
        true: if the link is active
        false: all other times
*/
bool DRV_ENCX24J600_LinkCheck(DRV_HANDLE hMac)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)hMac;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return false;
    }

    return pDrvInst->mainStateInfo.runningInfo.chkStaInfo.linkState;
}

// *****************************************************************************
/* ENCX24J600 Receive Filter Hash Table Entry Set

    Summary:
    This function adds an entry to the hash table

    Details:
    This function adds to the MACâ??s hash table for hash table matching.
    Note: This functionality isnâ??t implemented in the first release.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle
        DestMACAddr: MAC address to add to the hash table.

    Returns:
        TCPIP_MAC_RES_TYPE_ERR: if the hMac is invalid
        TCPIP_MAC_RES_OP_ERR: otherwise
 */
TCPIP_MAC_RES DRV_ENCX24J600_RxFilterHashTableEntrySet(DRV_HANDLE hMac, const TCPIP_MAC_ADDR* DestMACAddr)
{
    return TCPIP_MAC_RES_OP_ERR;
}

// *****************************************************************************
/* ENCX24J600 Power Mode

    Summary:
    This function sets the power mode of the device.

    Details:
    This function sets the power mode of the ENCx24J600.  Note: This functionality
    isnâ??t implemented in the first release.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle
        pwrMode: The power mode to set

    Returns:
        false: This functionality is not supported in this version of the driver
*/
bool  DRV_ENCX24J600_PowerMode(DRV_HANDLE hMac, TCPIP_MAC_POWER_MODE pwrMode)
{
    return false;
}

// *****************************************************************************
/* ENCX24J600 Packet Transmit

    Summary:
    This function queues a packet for transmission

    Details:
    This function will take a packet and add it to the queue for transmission.
    When the packet has finished transmitting the driver will call the packets
    acknowledge function.  When that acknowledge function is complete the driver
    will forget about the packet.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters
        hMac: the successfully opened handle
        ptrPacket: pointer to the packet

    Returns:
        TCPIP_MAC_RES_OP_ERR: if the client handle is invalid
        TCPIP_MAC_RES_IS_BUSY: if the driver is not in the run state
        TCPIP_MAC_RES_QUEUE_TX_FULL: if there are no free descriptors
        TCPIP_MAC_RES_OK: on successful queuing of the packet
*/
TCPIP_MAC_RES DRV_ENCX24J600_PacketTx(DRV_HANDLE hMac, TCPIP_MAC_PACKET * ptrPacket)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)hMac;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }

    TCPIP_Helper_ProtectedSingleListTailAdd(&pDrvInst->txPendingPackets, (SGL_LIST_NODE*)ptrPacket);
    ptrPacket->pktFlags |= TCPIP_MAC_PKT_FLAG_QUEUED;
    return TCPIP_MAC_RES_OK;
}


// *****************************************************************************
/* ENCX24J600 Receive Packet

    Summary:
    Receive a packet from the driver

    Details:
    This function retrieves a packet from the driver.  The packet needs to be
    acknowledged with the linked acknowledge function so it can be reused.
    Note: pPktStat is ignored in the first release.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle
        pRes: the result of the operation
        pPktStat: address to the receive statistics

    Returns:
        pointer to a valid packet on success
        NULL otherwise
*/
TCPIP_MAC_PACKET* DRV_ENCX24J600_PacketRx(DRV_HANDLE hMac, TCPIP_MAC_RES* pRes, TCPIP_MAC_PACKET_RX_STAT* pPktStat)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)hMac;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return NULL;
    }
    if (TCPIP_Helper_ProtectedSingleListIsEmpty(&pDrvInst->rxWaitingForPickupPackets))
    {
        return NULL;
    }
    return (TCPIP_MAC_PACKET*)TCPIP_Helper_ProtectedSingleListHeadRemove(&pDrvInst->rxWaitingForPickupPackets);


}

// *****************************************************************************
/* ENCX24J600 Process

    Summary:
    Additional processing that happens outside the tasks function

    Details:
    This function does additional processing that isnâ??t done inside the tasks function.  Note: This function does nothing in the first release.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle

    Returns:
        TCPIP_MAC_RES_TYPE_ERR: if the hMac is invalid
        TCPIP_MAC_RES_OP_ERR: otherwise
*/
TCPIP_MAC_RES DRV_ENCX24J600_Process(DRV_HANDLE hMac)
{
    return TCPIP_MAC_RES_OP_ERR;
}


// *****************************************************************************
/* ENCX24J600 Get Statistics

    Summary:
    Retrieve the devices statistics

    Details:
    Get the current statistics stored in the driver.  Note: Statistics are not
    planned for the first release.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle

    Returns:
        TCPIP_MAC_RES_TYPE_ERR: if the hMac is invalid
        TCPIP_MAC_RES_OP_ERR: otherwise
*/
TCPIP_MAC_RES DRV_ENCX24J600_StatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_RX_STATISTICS* pRxStatistics, TCPIP_MAC_TX_STATISTICS* pTxStatistics)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)hMac;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }
    if (pRxStatistics != NULL)
    {
        pRxStatistics->nRxOkPackets = pDrvInst->mainStateInfo.runningInfo.nRxOkPackets;
        pRxStatistics->nRxPendBuffers = pDrvInst->drvCfg.rxDescriptors - TCPIP_Helper_ProtectedSingleListCount(&pDrvInst->rxFreePackets);
        pRxStatistics->nRxSchedBuffers = TCPIP_Helper_ProtectedSingleListCount(&pDrvInst->rxFreePackets);
        pRxStatistics->nRxBuffNotAvailable = -1; //not Implemented
        pRxStatistics->nRxFragmentErrors = -1; //not Implemented
        pRxStatistics->nRxErrorPackets = -1; //not Implemented
    }
    if (pTxStatistics != NULL)
    {
        pTxStatistics->nTxOkPackets = pDrvInst->mainStateInfo.runningInfo.nTxOkPackets;
        pTxStatistics->nTxPendBuffers = TCPIP_Helper_ProtectedSingleListCount(&pDrvInst->txPendingPackets);
        pTxStatistics->nTxErrorPackets = -1; //not Implemented
        pTxStatistics->nTxQueueFull = -1; //not Implemented
        uint8_t count = 0;
        for (count = 0; count < MAX_TX_DESCRIPTORS; count++)
        {
            if (pDrvInst->txDescriptors[count].state != DRV_ENCX24J600_TP_NO_PKT_STATE)
            {
                pTxStatistics->nTxPendBuffers++;
            }
        }
    }

    return TCPIP_MAC_RES_OK;
}


// *****************************************************************************
/* ENCX24J600 Get Parameters

    Summary:
    Get the parameters of the device

    Details:
    Get the parameters of the device, this includes that it is an Ethernet device
    and what itâ??s MAC address is.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle
        pMacParams: pointer to put the parameters

    Returns:
        TCPIP_MAC_RES_TYPE_ERR: if the hMac is invalid
        TCPIP_MAC_RES_OK: otherwise
*/
TCPIP_MAC_RES DRV_ENCX24J600_ParametersGet(DRV_HANDLE hMac, TCPIP_MAC_PARAMETERS* pMacParams)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)hMac;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return TCPIP_MAC_RES_OP_ERR;
    }
    memcpy(pMacParams, &pDrvInst->stackParameters, sizeof(TCPIP_MAC_PARAMETERS));
    return TCPIP_MAC_RES_OK;
}


// *****************************************************************************
/* ENCX24J600 Get Register Statistics

    Summary:
    Get the register statistics

    Details:
    Get the device specific statistics.  Note: Statistics are not planned for
    the first release

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle
        pRegEntries
        nEntries
        pHwEntries

    Returns:
        TCPIP_MAC_RES_TYPE_ERR: if the hMac is invalid
        TCPIP_MAC_RES_OP_ERR: otherwise
*/
TCPIP_MAC_RES DRV_ENCX24J600_RegisterStatisticsGet(DRV_HANDLE hMac, TCPIP_MAC_STATISTICS_REG_ENTRY* pRegEntries, int nEntries, int* pHwEntries)
{
    return TCPIP_MAC_RES_OP_ERR;
}

// *****************************************************************************
/* ENCX24J600 Get Configuration

    Summary:
    Gets the current configuration

    Details:
    Gets the current configuration.  Note: This function does nothing in the
    first release.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle
        configBuff: location to copy the configuration too
        buffSize: buffer size
        pConfigSize: configuration size needed

    Returns:
        Number of bytes copied to the buffer
*/
size_t DRV_ENCX24J600_ConfigGet(DRV_HANDLE hMac, void* configBuff, size_t buffSize, size_t* pConfigSize)
{
    return 0;
}

// *****************************************************************************
/* ENCX24J600 Set Event Mask

    Summary:
    Sets the event mask

    Details:
    Sets the event mask to what is passed in.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle
        macEvents: the mask to enable or disable
        enable: to enable or disable events

    Returns
        true if the mask could be set
        false otherwise
*/
bool DRV_ENCX24J600_EventMaskSet(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents, bool enable)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)hMac;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return false;
    }
    pDrvInst->eventMask = macEvents;


    return true;
}

// *****************************************************************************
/* ENCX24J600 Acknowledge Event

    Summary:
    Acknowledges an event

    Details:
    This function acknowledges an event.

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle
        macEvents: the events to acknowledge

    Returns:
        true: if successful
        false: otherwise
*/
bool DRV_ENCX24J600_EventAcknowledge(DRV_HANDLE hMac, TCPIP_MAC_EVENT macEvents)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)hMac;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return false;
    }
    OSAL_RESULT res = OSAL_MUTEX_Lock(&pDrvInst->drvMutex, OSAL_WAIT_FOREVER);
    res = res; // Force ignore of unused variable, which will be used if SYS_ASSERT was used

    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not lock driver mutex");

    pDrvInst->currentEvents &= ~macEvents;

    if (!TCPIP_Helper_ProtectedSingleListIsEmpty(&pDrvInst->rxWaitingForPickupPackets))
    {
        pDrvInst->currentEvents |= pDrvInst->eventMask & TCPIP_MAC_EV_RX_DONE;
    }

    res = OSAL_MUTEX_Unlock(&pDrvInst->drvMutex);
    SYS_ASSERT(res == OSAL_RESULT_TRUE, "Could not unlock driver mutex");


    return true;
}

// *****************************************************************************
/* ENCX24J600 Get Events

    Summary:
    Gets the current events

    Details:
    This function gets the current events

    Preconditions:
    The client had to be successfully open with DRV_ENCX24J600_Open()

    Parameters:
        hMac: the successfully opened handle

    Returns
        TCPIP_MAC_EV_NONE: on Error
        List of events otherwise
*/
TCPIP_MAC_EVENT DRV_ENCX24J600_EventPendingGet(DRV_HANDLE hMac)
{
    DRV_ENCX24J600_ClientInfo * pClient = (DRV_ENCX24J600_ClientInfo *)hMac;
    DRV_ENCX24J600_DriverInfo * pDrvInst = _DRV_ENCX24J600_ValidateClientHandle(pClient);
    if (pDrvInst == NULL)
    {
        return TCPIP_MAC_EV_NONE;
    }
    return pDrvInst->currentEvents;
}
