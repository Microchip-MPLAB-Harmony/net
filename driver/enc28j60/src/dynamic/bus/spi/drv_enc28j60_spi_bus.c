/*******************************************************************************
  ENC28J60 Driver SPI Bus Interface
  Company:
    Microchip Technology Inc.

  File Name:
    drv_enc28j60_spi_bus.c
  Summary:

  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*
Copyright (C) 2015-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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

// DOM-IGNORE-END

#include "drv_enc28j60_spi_bus.h"

// PHY supported operations
typedef enum
{
    DRV_ENC28J60_PHY_OP_WRITE_START,    // start write operation
    DRV_ENC28J60_PHY_OP_READ_START,     // start read operation
    DRV_ENC28J60_PHY_OP_WRITE,          // write operation in progress
    DRV_ENC28J60_PHY_OP_READ,           // read operation in progress
}DRV_ENC28J60_PHY_OP;

// PHY operation current state
typedef enum
{
    DRV_OP_STATE_IDLE,             // no operation in progress
    DRV_OP_STATE_START_MIREGADDR,  // start MIREGADR transaction
    DRV_OP_STATE_WAIT_MIREGADDR,   // wait for MIREGADR completion
    DRV_OP_STATE_START_MIIRD_MIWR,  // start MIRD/MIWRLH transaction
    DRV_OP_STATE_WAIT_MIIRD_MIWR,  // wait for MIRD/MIWRLH completion
    DRV_OP_STATE_WAIT_TMO,         // wait for 10.24us to pass
    DRV_OP_STATE_POLL_BUSY,        // poll MISTAT.BUSY to clear itself
    DRV_OP_STATE_WAIT_BUSY,        // wait for MISTAT.BUSY to clear itself
    DRV_OP_STATE_START_MICMD,      // start MICMD.MIIRD transaction
    DRV_OP_STATE_WAIT_MICMD,       // wait for MICMD.MIIRD completion - read op only
    DRV_OP_STATE_START_MIRD,       // start MIRDL/H transaction
    DRV_OP_STATE_WAIT_MIRD,        // wait for MIRDL/H completion - read op only
}DRV_ENC28J60_PHY_OP_STATE;


// descriptor for an ongoing PHY operation
typedef struct
{
    uint16_t                currOp;         // DRV_ENC28J60_PHY_OP: current operation
    uint16_t                currState;      // DRV_ENC28J60_PHY_OP_STATE: current state
    uint16_t                currReg;        // DRV_ENC28J60_PHY_SFR_MAP: current register operation
    uint16_t                wrValue;        // the value to be written
    uint16_t                rdValue;        // the value that was read
    uint16_t                retryCount;     // if lack of resources, operations are retried locally
    uintptr_t               currOpDcpt;     // descriptor for the current operation
    uint32_t                startTick;      // tick when operation was initiated
    uint32_t                totRetryCnt;    // statistics, debug
}DRV_ENC28J60_PHY_OP_DCPT;

static DRV_ENC28J60_PHY_OP_DCPT phyOpDcpt = { 0 };

static const DRV_ENC28J60_BusVTable drv_ENC28J60_spi_vtable =
{
    .fpOpenIf = &DRV_ENC28J60_SPI_OpenInterface,
    .fpCloseIf = &DRV_ENC28J60_SPI_CloseInterface,
    .fpOpResult = &DRV_ENC28J60_SPI_OperationResult,
    .fpSfrWr = &DRV_ENC28J60_SPI_SfrWrite,
    .fpSfrWr16 = &DRV_ENC28J60_SPI_SfrWrite16,
    .fpSfrRdStart = &DRV_ENC28J60_SPI_SfrReadStart,
    .fpSfrRdResult = &DRV_ENC28J60_SPI_SfrReadResult,
    .fpSfrRd16Start = &DRV_ENC28J60_SPI_SfrRead16Start,
    .fpSfrRd16Result = &DRV_ENC28J60_SPI_SfrRead16Result,
    .fpSfrBitSet = &DRV_ENC28J60_SPI_SfrBitSet,
    .fpSfrBitClr = &DRV_ENC28J60_SPI_SfrBitClear,
    .fpSysRst = &DRV_ENC28J60_SPI_SystemReset,
    .fpRxEnable = &DRV_ENC28J60_SPI_EnableRX,
    .fpRxDisable = &DRV_ENC28J60_SPI_DisableRX,
    .fpDecPktCnt = &DRV_ENC28J60_SPI_DecrPktCtr,
    .fpIntEnable = &DRV_ENC28J60_SPI_EnableInterrupts,
    .fpIntDisable = &DRV_ENC28J60_SPI_DisableInterrupts,
    .fpFlowCtrlDisable = &DRV_ENC28J60_SPI_FlowCtrlDisable,
    .fpFlowCtrlSingle = &DRV_ENC28J60_SPI_FlowCtrlSingle,
    .fpFlowCtrlMult = &DRV_ENC28J60_SPI_FlowCtrlMult,
    .fpFlowCtrlClr = &DRV_ENC28J60_SPI_FlowCtrClear,
    .fpPhyWrStart = &DRV_ENC28J60_SPI_PhyWriteStart,
    .fpPhyWr = &DRV_ENC28J60_SPI_PhyWrite,
    .fpPhyRdStart = &DRV_ENC28J60_SPI_PhyReadStart,
    .fpPhyRd = &DRV_ENC28J60_SPI_PhyRead,
    .fpPktWr = &DRV_ENC28J60_SPI_WritePacket,
    .fpDataRdStart = &DRV_ENC28J60_SPI_ReadDataStart,
};

// helpers, conversion functions
//
static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_ExtBuff2Node(DRV_ENC28J60_OP_EXT_BUFF* pExtBuff)
{
    union
    {
        DRV_ENC28J60_OP_EXT_BUFF* pExtBuff;
        SGL_LIST_NODE* node;
    }U_EXT_BUFF_SGL_NODE;

    U_EXT_BUFF_SGL_NODE.pExtBuff = pExtBuff;
    return U_EXT_BUFF_SGL_NODE.node;
}

static __inline__ DRV_ENC28J60_OP_EXT_BUFF* __attribute__((always_inline)) FC_Node2ExtBuff(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE* node;
        DRV_ENC28J60_OP_EXT_BUFF* pExtBuff;
    }U_SGL_NODE_EXT_BUFF;

    U_SGL_NODE_EXT_BUFF.node = node;
    return U_SGL_NODE_EXT_BUFF.pExtBuff;
}

static __inline__ SGL_LIST_NODE* __attribute__((always_inline)) FC_OpDcpt2Node(DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    union
    {
        DRV_ENC28J60_OP_DCPT* pOpDcpt;
        SGL_LIST_NODE* node;
    }U_OP_DCPT_SGL_NODE;

    U_OP_DCPT_SGL_NODE.pOpDcpt = pOpDcpt;
    return U_OP_DCPT_SGL_NODE.node;
}

static __inline__ DRV_ENC28J60_OP_DCPT* __attribute__((always_inline)) FC_Node2OpDcpt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE* node;
        DRV_ENC28J60_OP_DCPT* pOpDcpt;
    }U_SGL_NODE_OP_DCPT;

    U_SGL_NODE_OP_DCPT.node = node;
    return U_SGL_NODE_OP_DCPT.pOpDcpt;
}

static __inline__ DRV_ENC28J60_OP_DCPT* __attribute__((always_inline)) FC_Handle2OpDcpt(uintptr_t handle)
{
    union
    {
        uintptr_t handle;
        DRV_ENC28J60_OP_DCPT* pOpDcpt;
    }U_HANDLE_OP_DCPT;

    U_HANDLE_OP_DCPT.handle = handle;
    return U_HANDLE_OP_DCPT.pOpDcpt;
}


// local prototypes
static DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyProcess(DRV_ENC28J60_DriverInfo* pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg, DRV_ENC28J60_PHY_OP currOp);
static void F_DRV_ReleaseOpDcpt(DRV_ENC28J60_spiBusData* pBusData, DRV_ENC28J60_OP_DCPT* pOpDcpt);
static DRV_ENC28J60_BUS_RESULT F_DRV_ENC28J60_OpStatusToResult(DRV_ENC28J60_OP_DCPT* pOpDcpt);
static bool F_DRV_ENC28J60_DoBankSet(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP sfr);
static uintptr_t DRV_SfrBitSet_NoBank(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  regValue, bool autoAck);
static uintptr_t DRV_SfrBitClear_NoBank(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  regValue, bool autoAck);

static void F_DRV_AutoAcknowledge(DRV_ENC28J60_spiBusData* pBusData)
{
    DRV_ENC28J60_BUS_RESULT res;
    SINGLE_LIST             newAutoList;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;

    TCPIP_Helper_SingleListInitialize(&newAutoList);
    while( (pOpDcpt = FC_Node2OpDcpt(TCPIP_Helper_SingleListHeadRemove(&pBusData->opListAutoAck))) != NULL)
    {   // even if it's linked we process it separately: it won't be queried!
        if(pOpDcpt->spiHandle != 0)
        {   // if the handle is 0, it was just added to the list
            res = F_DRV_ENC28J60_OpStatusToResult(pOpDcpt);
            if(res != DRV_ENC28J60_BR_PENDING)
            {   // completed
                F_DRV_ReleaseOpDcpt(pBusData, pOpDcpt);
                continue;
            }
        }

        // still busy or doesn't have a buffer
        TCPIP_Helper_SingleListTailAdd(&newAutoList, FC_OpDcpt2Node(pOpDcpt));
    }
    TCPIP_Helper_SingleListAppend(&pBusData->opListAutoAck, &newAutoList);

}

static  DRV_ENC28J60_OP_DCPT* F_DRV_GetOpDcpt(DRV_ENC28J60_spiBusData* pBusData, bool autoAck, uint8_t opFlags)
{
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;

    // check first if we need to auto acknowledge
    F_DRV_AutoAcknowledge(pBusData);


    // grab a free node
    pOpDcpt = FC_Node2OpDcpt(TCPIP_Helper_SingleListHeadRemove(&pBusData->opFreeList));
    
    if(pOpDcpt != NULL)
    {
        pOpDcpt->next = NULL;  // show it's not in any list;
        pOpDcpt->link = NULL;  // show it's not linked;
        pOpDcpt->spiHandle = 0;
        pOpDcpt->extBuffer = NULL;
        pOpDcpt->opStatus = (uint8_t)DRV_ENC28J60_OP_STAT_BUSY;
        pOpDcpt->opFlags = autoAck ? opFlags | (uint8_t)DRV_ENC28J60_OP_FLAG_AUTO_ACK : opFlags;
        pBusData->opDcptOkCnt++;
    }
    else
    {
        pBusData->opDcptFailCnt++;
    }
    return pOpDcpt;
} 

static __inline__ DRV_ENC28J60_OP_EXT_BUFF* __attribute__((always_inline)) F_DRV_GetExtBuffer(DRV_ENC28J60_spiBusData* pBusData)
{
    DRV_ENC28J60_OP_EXT_BUFF* pExtBuff = FC_Node2ExtBuff(TCPIP_Helper_SingleListHeadRemove(&pBusData->extBuffFreeList));

    if(pExtBuff != NULL)
    {
        pBusData->extBuffOkCnt++;
    }
    else
    {
        pBusData->extBuffFailCnt++;
    }

    return pExtBuff;
}

// regular release
static void F_DRV_ReleaseOpDcpt(DRV_ENC28J60_spiBusData* pBusData, DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    if(pOpDcpt->extBuffer != NULL)
    {
        TCPIP_Helper_SingleListTailAdd(&pBusData->extBuffFreeList, FC_ExtBuff2Node(pOpDcpt->extBuffer));
        pOpDcpt->extBuffer = NULL;
    }

    TCPIP_Helper_SingleListTailAdd(&pBusData->opFreeList, FC_OpDcpt2Node(pOpDcpt));
    pOpDcpt->opStatus = (uint8_t)DRV_ENC28J60_OP_STAT_IDLE;  // mark it as done
}

static void F_DRV_ReleaseOpDcptEx(DRV_ENC28J60_spiBusData* pBusData, DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    if(pOpDcpt->extBuffer != NULL)
    {
        TCPIP_Helper_SingleListTailAdd(&pBusData->extBuffFreeList, FC_ExtBuff2Node(pOpDcpt->extBuffer));
        pOpDcpt->extBuffer = NULL;
    }

    TCPIP_Helper_SingleListTailAdd(&pBusData->opFreeList, FC_OpDcpt2Node(pOpDcpt));
    pOpDcpt->opStatus = (uint8_t)DRV_ENC28J60_OP_STAT_IDLE;  // mark it as done
}

static DRV_ENC28J60_BUS_RESULT F_DRV_ENC28J60_OpStatusToResult(DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    DRV_SPI_TRANSFER_EVENT spiEv = DRV_SPI_TransferStatusGet (pOpDcpt->spiHandle);
    if((spiEv == DRV_SPI_TRANSFER_EVENT_COMPLETE) || (spiEv == DRV_SPI_TRANSFER_EVENT_HANDLE_EXPIRED))
    {   // success
        pOpDcpt->opStatus = (uint8_t)DRV_ENC28J60_OP_STAT_SUCCESS;
        return DRV_ENC28J60_BR_SUCCESS;
    }
    else if(spiEv == DRV_SPI_TRANSFER_EVENT_ERROR)
    {
        pOpDcpt->opStatus = (uint8_t)DRV_ENC28J60_OP_STAT_ERROR;
        return DRV_ENC28J60_BR_ERROR;
    }
    else
    {   // still busy
        DRV_ENC28J60_Assert(pOpDcpt->opStatus == (uint8_t)DRV_ENC28J60_OP_STAT_BUSY, __func__, __LINE__);
        return DRV_ENC28J60_BR_PENDING;
    }
}

static DRV_ENC28J60_BUS_RESULT F_DRV_ENC28J60_LinkedOpStatus(DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    DRV_ENC28J60_BUS_RESULT res;
    bool    isError;


    isError = false;
    for( ; pOpDcpt != NULL; pOpDcpt = pOpDcpt->link)
    {
        res = F_DRV_ENC28J60_OpStatusToResult(pOpDcpt);
        if(res == DRV_ENC28J60_BR_PENDING)
        {   // not all descriptors ready
            return DRV_ENC28J60_BR_PENDING;
        }
        else if(res == DRV_ENC28J60_BR_ERROR)
        {
            isError = true;
        }
        else
        {
            // OK
        }
    }

    return isError ? DRV_ENC28J60_BR_ERROR : DRV_ENC28J60_BR_SUCCESS;
}

static void F_DRV_ReleaseLinkedOpDcpt(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    DRV_ENC28J60_OP_DCPT*   pDcpt;

    while(pOpDcpt != NULL)
    {
        pDcpt = pOpDcpt->link;
        F_DRV_ReleaseOpDcptEx((DRV_ENC28J60_spiBusData *)pDrvInstance->busData, pOpDcpt);
        pOpDcpt = pDcpt;
    }

}

/*static void DRV_ENC28J60_SPI_OperationStarting (DRV_SPI_TRANSFER_EVENT event, DRV_SPI_TRANSFER_HANDLE bufferHandle, void * context )
{
    DRV_ENC28J60_DriverInfo*  pDrvInstance = (DRV_ENC28J60_DriverInfo*)context;
    PLIB_PORTS_PinClear(pDrvInstance->drvCfg.spiSSPortModule, pDrvInstance->drvCfg.spiSSPortChannel, pDrvInstance->drvCfg.spiSSPortPin);
}

- Note: each descriptor is released by itself when autoAck and added to the free list;
- the link between multiple linked descriptors should not be needed anymore
- because the autoAck operation cannot ask for status completion!
static void DRV_ENC28J60_SPI_OperationEnded (DRV_SPI_TRANSFER_EVENT event, DRV_SPI_TRANSFER_HANDLE bufferHandle, void * context )
{
    if(event == DRV_SPI_BUFFER_EVENT_COMPLETE || event == DRV_SPI_BUFFER_EVENT_ERROR)
    {
        DRV_ENC28J60_DriverInfo*  pDrvInstance = (DRV_ENC28J60_DriverInfo*)context;
        PLIB_PORTS_PinSet(pDrvInstance->drvCfg.spiSSPortModule, pDrvInstance->drvCfg.spiSSPortChannel, pDrvInstance->drvCfg.spiSSPortPin);

    }
}*/

// this function sets the proper spi handle for an operation
// it also adds the operation descriptor to the proper list
static void F_DRV_OpSetSpiHandle(DRV_ENC28J60_spiBusData* pBusData, DRV_ENC28J60_OP_DCPT* pOpDcpt, DRV_SPI_TRANSFER_HANDLE spiHandle, bool autoAck)
{
    DRV_ENC28J60_Assert(spiHandle != 0 && pOpDcpt->spiHandle == 0, __func__, __LINE__);
    pOpDcpt->spiHandle = spiHandle;
    // internal ops have autoAck: DoBank!
    if(autoAck)
    {
        TCPIP_Helper_SingleListTailAdd(&pBusData->opListAutoAck, FC_OpDcpt2Node(pOpDcpt));
    }
}

static DRV_ENC28J60_OP_DCPT* F_DRV_OpHandleFromUserHandle(uintptr_t  handle, bool isLinked)
{
    bool cond;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt = FC_Handle2OpDcpt(handle);

    if(isLinked)
    {   // possible that no all ops are done; but at least one should be busy
        cond = (pOpDcpt != NULL && pOpDcpt->link != NULL && (pOpDcpt->opStatus == (uint8_t)DRV_ENC28J60_OP_STAT_BUSY || pOpDcpt->link->opStatus == (uint8_t)DRV_ENC28J60_OP_STAT_BUSY));
        // for now linked nodes don't support autoAck
        if(pOpDcpt->next != NULL)
        {
            cond = 0;
        }
    }
    else
    {
        cond = (pOpDcpt != NULL && pOpDcpt->opStatus == (uint8_t)DRV_ENC28J60_OP_STAT_BUSY);
    }

    DRV_ENC28J60_Assert(cond, __func__, __LINE__);

    return cond ? pOpDcpt : NULL;
}

/*static void DRV_ENC28J60_SPI_Acknowledge (DRV_SPI_TRANSFER_EVENT event, DRV_SPI_TRANSFER_HANDLE bufferHandle, void * context )
{
}*/




int32_t DRV_ENC28J60_SPI_InitializeInterface(DRV_ENC28J60_DriverInfo *  pDrvInstance )
{
    int ix;
    DRV_ENC28J60_OP_DCPT*       pOpDcpt;
    DRV_ENC28J60_OP_EXT_BUFF*   pExtBuff; 

    DRV_ENC28J60_spiBusData * pBusData = (DRV_ENC28J60_spiBusData *)(*pDrvInstance->stackCfg.callocF)(pDrvInstance->stackCfg.memH, 1, sizeof(DRV_ENC28J60_spiBusData));
    if (pBusData == NULL)
    {   // failed
        return -1;
    }

    // success
    pDrvInstance->busData = pBusData;
    pDrvInstance->busVTable = &drv_ENC28J60_spi_vtable;
    pBusData->clientHandle = DRV_HANDLE_INVALID;
    pBusData->currentBank = 0xff;  // Some invalid bank number
    // construct the operations and external buffers lists
    TCPIP_Helper_SingleListInitialize(&pBusData->opFreeList);
    TCPIP_Helper_SingleListInitialize(&pBusData->opListAutoAck);
    TCPIP_Helper_SingleListInitialize(&pBusData->extBuffFreeList);
    pOpDcpt = pBusData->opArray;
    for(ix = 0; ix < sizeof(pBusData->opArray) / sizeof(*pBusData->opArray); ix++)
    {
        pOpDcpt->pDrvInstance = pDrvInstance;
        TCPIP_Helper_SingleListTailAdd(&pBusData->opFreeList, FC_OpDcpt2Node(pOpDcpt));
        pOpDcpt++;
    }
    pExtBuff =  pBusData->extBuffArray;
    for(ix = 0; ix < sizeof(pBusData->extBuffArray) / sizeof(*pBusData->extBuffArray); ix++)
    {
        TCPIP_Helper_SingleListTailAdd(&pBusData->extBuffFreeList, FC_ExtBuff2Node(pExtBuff));
        pExtBuff++;
    }
    pBusData->extBuffSize = sizeof(pExtBuff->extBuffer) - DRV_ENC28J60_OP_EXT_BUFFER_EXTRA_SIZE; 

    (void)memset(&phyOpDcpt, 0, sizeof(phyOpDcpt));

    return 0;
}

int32_t DRV_ENC28J60_SPI_DeinitializeInterface(DRV_ENC28J60_DriverInfo *  pDrvInstance)
{

    if(pDrvInstance && pDrvInstance->busData)
    {
        DRV_ENC28J60_spiBusData* pBusData = pDrvInstance->busData;
        // make sure everything is cleared
        DRV_ENC28J60_Assert(TCPIP_Helper_SingleListCount(&pBusData->opFreeList) == sizeof(pBusData->opArray) / sizeof(*pBusData->opArray), __func__, __LINE__);
        DRV_ENC28J60_Assert(TCPIP_Helper_SingleListCount(&pBusData->extBuffFreeList) == sizeof(pBusData->extBuffArray) / sizeof(*pBusData->extBuffArray), __func__, __LINE__);

        (void)(*pDrvInstance->stackCfg.freeF)(pDrvInstance->stackCfg.memH, pDrvInstance->busData);
        pDrvInstance->busData = NULL;
        pDrvInstance->busVTable = NULL;
    }
    return 0;
}

// *****************************************************************************
/* Open

    Summary:
    Opens the bus interface

    Details:
    This function call opens the bus interface.

    Preconditions:
    The bus has to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns
        Negative if error
        Valid Handle on success
*/
int32_t DRV_ENC28J60_SPI_OpenInterface(DRV_ENC28J60_DriverInfo *  pDrvInstance )
{
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;
    pBusInfo->clientHandle = DRV_SPI_Open(pDrvInstance->drvCfg.spiDrvIndex, DRV_IO_INTENT_READWRITE);
    if (pBusInfo->clientHandle == DRV_HANDLE_INVALID)
    {
        return -1;
    }
      
    (void)DRV_SPI_TransferSetup (pBusInfo->clientHandle, &pDrvInstance->drvCfg.spiSetup);
    return 0;
}

// *****************************************************************************
/* Close

    Summary:
    Closes the bus interface

    Details:
    This function call closes the bus interface.

    Preconditions:
    The bus has to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance

    Returns:
        None
*/
void DRV_ENC28J60_SPI_CloseInterface( DRV_ENC28J60_DriverInfo *  pDrvInstance)
{
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;
    if (pBusInfo->clientHandle == DRV_HANDLE_INVALID)
    {
        return;
    }
    DRV_SPI_Close(pBusInfo->clientHandle);
    pBusInfo->clientHandle = DRV_HANDLE_INVALID;
}

// *****************************************************************************
/* Operation Result

    Summary
    Checks the status of an operation on the bus.

    Details
    This function checks the status of an operation that was previously start.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        handle - Handle created by the operation.

    Returns
        DRV_ENC28J60_BR_SUCCESS �?? if the operation was successful
        DRV_ENC28J60_BR_PENDING �?? if the operation is still pending
        DRV_ENC28J60_BR_ERROR �?? if there was an error in the operation

   Remarks:
        Supports linked descriptors
*/
DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_OperationResult( DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, bool ack )
{
    DRV_ENC28J60_BUS_RESULT res;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt = F_DRV_OpHandleFromUserHandle(handle, false);

    if(pOpDcpt == NULL)
    {   // no such descriptor
        return DRV_ENC28J60_BR_INVALID;
    }

    res = F_DRV_ENC28J60_LinkedOpStatus(pOpDcpt);

    if(res != DRV_ENC28J60_BR_PENDING && ack)
    {   // completed
        F_DRV_ReleaseLinkedOpDcpt(pDrvInstance, pOpDcpt);
    }

    return res;
}

// selects a new register bank
// returns true if succes or fail and needs retry
static bool F_DRV_ENC28J60_DoBankSet(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP sfr)
{
    uint8_t     sfrBank;
    bool        success;
    DRV_ENC28J60_RegUnion regEcon1;

    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;


    sfrBank =  (uint8_t)sfr / 0x20U;   // ENCJ60 bank size
    DRV_ENC28J60_Assert(sfrBank < 4, __func__, __LINE__);

    if( (DRV_ENC28J60_SFR_EIE <= sfr && sfr <= DRV_ENC28J60_SFR_ECON1) || sfrBank == pBusInfo->currentBank )
    {    // either EIE, EIR, ECON1, ECON2 ESTAT regs or the right bank already selected
        return true;
    }

    success = false;
    regEcon1.value = 3;     // select both banks
    switch (sfrBank)
    {
        case 0:
            // clear both banks
            success = DRV_SfrBitClear_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0;
            break;
            
        case 1:
            // clear both banks
            if(DRV_SfrBitClear_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0)
            {
                pBusInfo->currentBank = 0;  // just in case the next op fails
                regEcon1.value = 1;     // set bank 1
                success = DRV_SfrBitSet_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0;
            }
            break;

        case 2:
            // clear both banks
            if(DRV_SfrBitClear_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0)
            {
                pBusInfo->currentBank = 0;  // just in case the next op fails
                regEcon1.value = 2;     // set bank 2
                success =  DRV_SfrBitSet_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0;
            }

            break;

        case 3:
            // set both banks
            success = DRV_SfrBitSet_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0;
            break;

        default:
            // should not happen
            break;
    }

    if(success)
    {
        pBusInfo->currentBank = sfrBank;
    }

    return success;

}


// *****************************************************************************
/* Special Function Register Write

    Summary:
    Starts a write operation to the special function register.

    Details:
    This function sends a value to the specified register. 

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance �?? The driver instance
        reg �?? The Special Function Register to write to.
        Value �?? the value to write into the register

    Returns:
        NULL �?? On Error
        Valid Handle �?? on success
*/

uintptr_t DRV_ENC28J60_SPI_SfrWrite(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfr, DRV_ENC28J60_RegUnion  regValue, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t*                pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if(F_DRV_ENC28J60_DoBankSet(pDrvInstance, sfr))
    {   // Put in the data
        if((pOpDcpt = F_DRV_GetOpDcpt(pBusInfo, autoAck, 0U)) != NULL)
        {   
            pWrBuff = pOpDcpt->opWrBuffer;
            *pWrBuff = (uint8_t)DRV_ENC28J60_SPI_INST_WCR | ((uint8_t)sfr & 0x1fU);
            *(pWrBuff + 1) = (uint8_t)regValue.value;
            //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
            DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff, 2UL, &spiHandle);

            
            if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
            {
                F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
                return (uintptr_t)pOpDcpt;
            }
            // couldn't launch the SPI op
            F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
        }
    }

    return 0;
}

// try a 16 bit operation as 2 independent 8 bit operations
uintptr_t DRV_ENC28J60_SPI_SfrWrite16(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfrLow, uint16_t regValue, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle1, spiHandle2;
    uint8_t                 *pWrBuff1, *pWrBuff2;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt1 = NULL;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt2 = NULL;
    DRV_ENC28J60_spiBusData *pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if(F_DRV_ENC28J60_DoBankSet(pDrvInstance, sfrLow))
    {   // Put in the data
        pOpDcpt1 = F_DRV_GetOpDcpt(pBusInfo, autoAck, (uint8_t)DRV_ENC28J60_OP_FLAG_LINKED);
        pOpDcpt2 = F_DRV_GetOpDcpt(pBusInfo, autoAck, (uint8_t)DRV_ENC28J60_OP_FLAG_LINKED);

        if(pOpDcpt1 && pOpDcpt2)
        {   // got both operations
            pWrBuff1 = pOpDcpt1->opWrBuffer;
            pWrBuff2 = pOpDcpt2->opWrBuffer;
            *pWrBuff1 = (uint8_t)DRV_ENC28J60_SPI_INST_WCR | ((uint8_t)sfrLow & 0x1fU);
            *(pWrBuff1 + 1) = (uint8_t)regValue;
            *pWrBuff2 = (uint8_t)DRV_ENC28J60_SPI_INST_WCR | (((uint8_t)sfrLow + 1U) & 0x1fU);
            *(pWrBuff2 + 1) = (uint8_t)(regValue >> 8);
            
            //spiHandle1 = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff1, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
            DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff1, 2UL, &spiHandle1);
            if(spiHandle1 != DRV_SPI_TRANSFER_HANDLE_INVALID)
            {
                //spiHandle2 = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff2, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
                DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff2, 2UL, &spiHandle2);
                if(spiHandle2 != DRV_SPI_TRANSFER_HANDLE_INVALID)
                {   // success
                    F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt1, spiHandle1, autoAck);
                    F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt2, spiHandle2, autoAck);
                    // link the descriptors
                    pOpDcpt1->link = pOpDcpt2;
                    return (uintptr_t)pOpDcpt1;
                }
                // couldn't launch the 2nd SPI op and the 1st one it's on its way
                // that's why this approach is better to be avoided..
                // mark it for auto deletion
                F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt1, spiHandle1, true);
                pOpDcpt1->opFlags |= (uint8_t)DRV_ENC28J60_OP_FLAG_AUTO_ACK;
                pOpDcpt1->opFlags &= ~(uint8_t)DRV_ENC28J60_OP_FLAG_LINKED;  // not linked anymore
                // make sure we don't release it now
                pOpDcpt1 = NULL;
            }
        }
    }

    if(pOpDcpt1 != NULL)
    {
        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt1);
    }
    if(pOpDcpt2 != NULL)
    {
        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt2);
    }

    return 0;
}




// *****************************************************************************
/* Special Function Register Read Start

    Summary:
    Starts a read operation from the special function register.

    Details
    This function starts a read operation of a value from the specified register.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance : The SPI driver instance
        reg : The Special Function Register to read from.

    Returns
        NULL : On Error
        Valid Handle : on success
*/
uintptr_t DRV_ENC28J60_SPI_SfrReadStart(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfr, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t*    pWrBuff;
    uint8_t*    pRdBuff;
    size_t      txferLen;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;

    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if(!F_DRV_ENC28J60_DoBankSet(pDrvInstance, sfr))
    {
        return 0;
    }

    if((pOpDcpt = F_DRV_GetOpDcpt(pBusInfo, autoAck, 0U)) != NULL)
    {
        pWrBuff = pOpDcpt->opWrBuffer;
        *pWrBuff = (uint8_t)DRV_ENC28J60_SPI_INST_RCR | ((uint8_t)sfr & 0x1fU);

        if ((DRV_ENC28J60_SFR_MACON1 <= sfr && sfr <= DRV_ENC28J60_SFR_MAADR2)  || sfr == DRV_ENC28J60_SFR_MISTAT)
        {   // read buffer is selected so that the 1 byte result will be in pOpDcpt->opRdBuffer + 2;
            pRdBuff = pOpDcpt->opRdBuffer;
            txferLen = 3;
        }
        else
        {
            pRdBuff = pOpDcpt->opRdBuffer + 1;
            txferLen = 2;
        }

        //spiHandle = DRV_SPI_BufferAddWriteRead(pBusInfo->clientHandle, pWrBuff, txferLen, pRdBuff, txferLen, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
        DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, pWrBuff, txferLen, pRdBuff, txferLen, &spiHandle);
        if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
            return (uintptr_t)pOpDcpt;
        }

        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    }

    return 0;
}

uintptr_t DRV_ENC28J60_SPI_SfrRead16Start(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfrLow, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle1, spiHandle2;
    uint8_t                 *pWrBuff1, *pWrBuff2;
    uint8_t                 *pRdBuff1, *pRdBuff2;
    size_t                  txferLen;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt1 = NULL;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt2 = NULL;

    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if(!F_DRV_ENC28J60_DoBankSet(pDrvInstance, sfrLow))
    {
        return 0;
    }

    pOpDcpt1 = F_DRV_GetOpDcpt(pBusInfo, autoAck, (uint8_t)DRV_ENC28J60_OP_FLAG_LINKED);
    pOpDcpt2 = F_DRV_GetOpDcpt(pBusInfo, autoAck, (uint8_t)DRV_ENC28J60_OP_FLAG_LINKED);

    if(pOpDcpt1 && pOpDcpt2)
    {   // got both operations
        pWrBuff1 = pOpDcpt1->opWrBuffer;
        pWrBuff2 = pOpDcpt2->opWrBuffer;

        *pWrBuff1 = (uint8_t)DRV_ENC28J60_SPI_INST_RCR | ((uint8_t)sfrLow & 0x1fU);    
        *pWrBuff2 = (uint8_t)DRV_ENC28J60_SPI_INST_RCR | (((uint8_t)sfrLow + 1U) & 0x1fU);  

        pRdBuff1 = pOpDcpt1->opRdBuffer;
        pRdBuff2 = pOpDcpt2->opRdBuffer;
        if ((DRV_ENC28J60_SFR_MACON1 <= sfrLow && sfrLow <= DRV_ENC28J60_SFR_MAADR2)  || sfrLow == DRV_ENC28J60_SFR_MISTAT)
        {   // read buffer is selected so that the 1 byte result will be in pOpDcpt->opRdBuffer + 2;
            txferLen = 3;
        }
        else
        {
            pRdBuff1++;
            pRdBuff2++;
            txferLen = 2;
        }

        //spiHandle1 = DRV_SPI_BufferAddWriteRead(pBusInfo->clientHandle, pWrBuff1, txferLen, pRdBuff1, txferLen, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
        DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, pWrBuff1, txferLen, pRdBuff1, txferLen, &spiHandle1);
        if(spiHandle1 != DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            //spiHandle2 = DRV_SPI_BufferAddWriteRead(pBusInfo->clientHandle, pWrBuff2, txferLen, pRdBuff2, txferLen, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
            DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, pWrBuff2, txferLen, pRdBuff2, txferLen, &spiHandle2);
            if(spiHandle2 != DRV_SPI_TRANSFER_HANDLE_INVALID)
            {   // success
                F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt1, spiHandle1, autoAck);
                F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt2, spiHandle2, autoAck);
                // link the descriptors
                pOpDcpt1->link = pOpDcpt2;
                return (uintptr_t)pOpDcpt1;
            }
            // couldn't launch the 2nd SPI op and the 1st one it's on its way
            // that's why this approach is better to be avoided..
            // mark it for auto deletion
            F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt1, spiHandle1, true);
            pOpDcpt1->opFlags |= (uint8_t)DRV_ENC28J60_OP_FLAG_AUTO_ACK;
            pOpDcpt1->opFlags &= ~(uint8_t)DRV_ENC28J60_OP_FLAG_LINKED;  // not linked anymore
            // make sure we don't release it now
            pOpDcpt1 = NULL;
        }
    }

    if(pOpDcpt1 != NULL)
    {
        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt1);
    }
    if(pOpDcpt2 != NULL)
    {
        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt2);
    }
    return 0;
}



// *****************************************************************************
/* Special Function Register Read Result

    Summary
    Gets the result of a read operation.

    Details
    This function checks the result of the read operation and if it was
    successful it will write the value into the output.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        handle �?? the handle from the read start operation
        value �?? where to put the results of the operation

    Returns
        DRV_ENC28J60_BR_SUCCESS �?? if the operation was successful
        DRV_ENC28J60_BR_PENDING �?? if the operation is still pending
        DRV_ENC28J60_BR_ERROR �?? if there was an error in the operation
*/
DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_SfrReadResult(DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, DRV_ENC28J60_RegUnion * regValue, bool ack)
{
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_BUS_RESULT res = DRV_ENC28J60_BR_INVALID;

    pOpDcpt = F_DRV_OpHandleFromUserHandle(handle, false);

    // make sure this is not an 16 bit op!
    if(pOpDcpt != NULL && pOpDcpt->link == NULL)
    {
        res = F_DRV_ENC28J60_LinkedOpStatus(pOpDcpt);

        if (res == DRV_ENC28J60_BR_SUCCESS)
        {
            regValue->value = pOpDcpt->opRdBuffer[2];
        }

        if(res != DRV_ENC28J60_BR_PENDING && ack)
        {
            F_DRV_ReleaseLinkedOpDcpt(pDrvInstance, pOpDcpt);
        }
    }

    return res;
}

DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_SfrRead16Result(DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, uint16_t* value, bool ack)
{
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_BUS_RESULT res = DRV_ENC28J60_BR_INVALID;

    pOpDcpt = F_DRV_OpHandleFromUserHandle(handle, true);

    // make sure this is an 16 bit op!
    if(pOpDcpt && pOpDcpt->link)
    {
        res = F_DRV_ENC28J60_LinkedOpStatus(pOpDcpt);

        if (res == DRV_ENC28J60_BR_SUCCESS)
        {
            *value = pOpDcpt->opRdBuffer[2] | ((uint16_t)pOpDcpt->link->opRdBuffer[2] << 8);
        }

        if(res != DRV_ENC28J60_BR_PENDING && ack)
        {   // completed
            F_DRV_ReleaseLinkedOpDcpt(pDrvInstance, pOpDcpt);
        }
    }
    return res;
}

// special function setting the SFR bits with no Bank setting
// use it for sfr in [DRV_ENC28J60_SFR_EIE, DRV_ENC28J60_SFR_ECON1] range
// for this range the F_DRV_ENC28J60_DoBankSet() does nothing!
static uintptr_t DRV_SfrBitSet_NoBank(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  regValue, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t *pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if((pOpDcpt = F_DRV_GetOpDcpt(pBusInfo, autoAck, 0U)) != NULL)
    {
        pWrBuff = pOpDcpt->opWrBuffer;

        // Put the Bit Field Set Command and SFR Adr 
        *pWrBuff = (uint8_t)DRV_ENC28J60_SPI_INST_BFS | ((uint8_t)reg & 0x1fU);
        // Put in the bit information to SET
        *(pWrBuff + 1) = (uint8_t)regValue.value & 0xffU;

        //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff, 2UL, &spiHandle);

        if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
            return (uintptr_t)pOpDcpt;
        }

        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    }
    return 0;
}

// *****************************************************************************
/* Special Function Register Bit Set

    Summary
    Sets a selection of bits in a special function register.

    Details
    This function will allow the setting of specific bits in a register without
    having to read the register first.  The bits turned on in value are the bits
    that get set.

    Preconditions
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        reg �?? The Special Function Register to write to.
        Value �?? The bits to set in the register.

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
uintptr_t DRV_ENC28J60_SPI_SfrBitSet(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  regValue, bool autoAck)
{
    // Select and Set the bank address for the SFR 
    if(F_DRV_ENC28J60_DoBankSet(pDrvInstance, reg))
    {
        return DRV_SfrBitSet_NoBank(pDrvInstance, reg, regValue, autoAck);
    }
    return 0;
}

// special function clearing the SFR bits with no Bank setting
// use it for sfr in [DRV_ENC28J60_SFR_EIE, DRV_ENC28J60_SFR_ECON1] range
// for this range the F_DRV_ENC28J60_DoBankSet() does nothing!
static uintptr_t DRV_SfrBitClear_NoBank(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  regValue, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t*    pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;


    if((pOpDcpt = F_DRV_GetOpDcpt(pBusInfo, autoAck, 0U)) != NULL)
    {
        pWrBuff = pOpDcpt->opWrBuffer;

        // Put the Bit Field Set Command and SFR Adr 
        *pWrBuff = (uint8_t)DRV_ENC28J60_SPI_INST_BFC | ((uint8_t)reg & 0x1fU);
        // Put in the bit information to CLR
        *(pWrBuff + 1) = (uint8_t)regValue.value & 0xffU;

        //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff, 2UL, &spiHandle);
        if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
            return (uintptr_t)pOpDcpt;
        }

        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    }

    return 0;
}
// *****************************************************************************
/* Special Function Register Bit Clear

    Summary:
    Clears a selection of bits in a special function register.

    Details:
    This function will allow the clearing of specific bits in a register without
    having to read the register first.  The bits turned on in value are the bits
    that get cleared.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance
        reg �?? The Special Function Register to write to.
        Value �?? The bits to clear in the register.

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
uintptr_t DRV_ENC28J60_SPI_SfrBitClear(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  regValue, bool autoAck)
{
    if(F_DRV_ENC28J60_DoBankSet(pDrvInstance, reg))
    {
        return DRV_SfrBitClear_NoBank(pDrvInstance, reg, regValue, autoAck);
    }

    return 0;
}

// *****************************************************************************
/* System Reset

    Summary:
    Sends a device reset.

    Details:
    This function sends the system reset command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance : The SPI driver instance

    Returns:
        NULL : On Error
        Valid Handle : on success
*/
   
uintptr_t DRV_ENC28J60_SPI_SystemReset(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t*    pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if((pOpDcpt = F_DRV_GetOpDcpt(pBusInfo, autoAck, 0U)) != NULL)
    {
        pWrBuff = pOpDcpt->opWrBuffer;

        *pWrBuff = (uint8_t)DRV_ENC28J60_SPI_INST_SRC;
        //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff, 1, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff, 1UL, &spiHandle);
        if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
            return (uintptr_t)pOpDcpt;
        }
        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    }

    return 0;
}

// *****************************************************************************
/* Enable Receive

    Summary:
    Sends an enable RX.

    Details:
    This function sends the Enable RX command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance �?? The driver instance

    Returns
        NULL �?? On Error
        Valid Handle �?? on success
*/
uintptr_t DRV_ENC28J60_SPI_EnableRX(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck )
{
    DRV_ENC28J60_RegUnion regEcon1 = {0};

    regEcon1.econ1.RXEN = 1;

    return DRV_SfrBitSet_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, autoAck);
}



// *****************************************************************************
/* Disable Receive

    Summary:
    Sends a disable RX.

    Details:
    This function sends the Disable RX command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance : The driver instance

    Returns
        NULL : On Error
        Valid Handle : on success
*/
uintptr_t DRV_ENC28J60_SPI_DisableRX(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck )
{
    DRV_ENC28J60_RegUnion regEcon1 = {0};

    regEcon1.econ1.RXEN = 1;

    return DRV_SfrBitClear_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, autoAck);
}

// *****************************************************************************
/* Decrement Packet Counter

    Summary:
    Sends a decrement packet counter.

    Details:
    This function sends the Decrement Packet command to the ENC28J60.  This is
    called by the RX state machine after it has received a packet.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance : The driver instance

    Returns
        NULL : On Error
        Valid Handle : on success
*/
 
uintptr_t DRV_ENC28J60_SPI_DecrPktCtr(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck)
{

    DRV_ENC28J60_RegUnion regEcon2 = {0};

    regEcon2.econ2.PKTDEC = 1;

    return DRV_SfrBitSet_NoBank(pDrvInstance, DRV_ENC28J60_SFR_ECON2, regEcon2, autoAck);
}

// *****************************************************************************
/* Enable Interrupts

    Summary:
    Sends an enable interrupts.

    Details:
    This function sends the Enable Interrupts command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters
        pDrvInstance : The driver instance

    Returns
        NULL : On Error
        Valid Handle : on success
*/
uintptr_t DRV_ENC28J60_SPI_EnableInterrupts(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck)
{
    // enable all interrupt bits except global INTIE
    DRV_ENC28J60_RegUnion regEie = {0x007b};

    return DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_EIE, regEie, autoAck);
}


// *****************************************************************************
/*  Disable Interrupts

    Summary:
    Sends a disable interrupts.

    Details:
    This function sends the Disable Interrupts command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance : The driver instance

    Returns:
        NULL : On Error
        Valid Handle : on success
*/
uintptr_t DRV_ENC28J60_SPI_DisableInterrupts(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck )
{
    // enable all interrupt bits
    DRV_ENC28J60_RegUnion regEie = {0x00fb};

    return DRV_ENC28J60_SPI_SfrBitClear(pDrvInstance, DRV_ENC28J60_SFR_EIE, regEie, autoAck);
}

uintptr_t DRV_ENC28J60_SPI_FlowCtrlDisable(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck )
{
    DRV_ENC28J60_RegUnion regEFloCon = {0};

    regEFloCon.eflocon.FCEN = 0x3;  

    return DRV_ENC28J60_SPI_SfrBitClear(pDrvInstance, DRV_ENC28J60_SFR_EFLOCON, regEFloCon, autoAck);
}

uintptr_t DRV_ENC28J60_SPI_FlowCtrlSingle(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck )
{
    DRV_ENC28J60_RegUnion regEFloCon = {0};

    regEFloCon.eflocon.FCEN = 0x1;  

    return DRV_ENC28J60_SPI_SfrWrite(pDrvInstance, DRV_ENC28J60_SFR_EFLOCON, regEFloCon, autoAck);
}

uintptr_t DRV_ENC28J60_SPI_FlowCtrlMult(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck )
{
    DRV_ENC28J60_RegUnion regEFloCon = {0};

    regEFloCon.eflocon.FCEN = 0x2;  

    return DRV_ENC28J60_SPI_SfrWrite(pDrvInstance, DRV_ENC28J60_SFR_EFLOCON, regEFloCon, autoAck);
}

// *****************************************************************************
/*  Clear Flow Control

    Summary:
    Sends a clear flow control.

    Details:
    This function sends the clear/terminate flow control command to the ENC28J60.

    Preconditions:
    The bus had to have been initialized first.

    Parameters:
        pDrvInstance : The driver instance

    Returns:
        NULL : On Error
        Valid Handle : on success
*/

uintptr_t DRV_ENC28J60_SPI_FlowCtrClear(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck )
{
    DRV_ENC28J60_RegUnion regEFloCon = {0};

    regEFloCon.eflocon.FCEN = 0x3;  

    return DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_EFLOCON, regEFloCon, autoAck);
}

// starts the write state machine
DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyWriteStart(struct S_DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP  reg, uint16_t regValue)
{
    if(phyOpDcpt.currState != (uint16_t)DRV_OP_STATE_IDLE)
    {
        return DRV_ENC28J60_PHY_RES_BUSY; 
    }

    phyOpDcpt.wrValue = regValue;

    return DRV_ENC28J60_SPI_PhyProcess(pDrvInstance, reg, DRV_ENC28J60_PHY_OP_WRITE_START);

}

// advances the write state machine
DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyWrite(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg)
{
    return DRV_ENC28J60_SPI_PhyProcess(pDrvInstance, reg, DRV_ENC28J60_PHY_OP_WRITE);
}

DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyReadStart(DRV_ENC28J60_DriverInfo* pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg)
{
    if(phyOpDcpt.currState != (uint16_t)DRV_OP_STATE_IDLE)
    {
        return DRV_ENC28J60_PHY_RES_BUSY; 
    }

    return DRV_ENC28J60_SPI_PhyProcess(pDrvInstance, reg, DRV_ENC28J60_PHY_OP_READ_START);
}

// advances the read state machine
DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyRead(DRV_ENC28J60_DriverInfo* pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg, uint16_t* phyRegValue)
{
    DRV_ENC28J60_PHY_RES phyRes = DRV_ENC28J60_SPI_PhyProcess(pDrvInstance, reg, DRV_ENC28J60_PHY_OP_READ);
    if(phyRes == DRV_ENC28J60_PHY_RES_OK)
    {
        *phyRegValue = phyOpDcpt.rdValue;
    }

    return phyRes;
}


static DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyProcess(DRV_ENC28J60_DriverInfo* pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg, DRV_ENC28J60_PHY_OP currOp)
{
    DRV_ENC28J60_PHY_RES    phyRes;
    DRV_ENC28J60_BUS_RESULT busRes;
    DRV_ENC28J60_RegUnion   phyReg;
    
    if(phyOpDcpt.currState == (uint16_t)DRV_OP_STATE_IDLE)
    {   // starting an operation

        phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_START_MIREGADDR;
        phyOpDcpt.currReg = (uint16_t)reg;
        phyOpDcpt.retryCount = 0;
       if(currOp == DRV_ENC28J60_PHY_OP_WRITE_START)
       {
           phyOpDcpt.currOp = (uint16_t)DRV_ENC28J60_PHY_OP_WRITE;
       }
       else if(currOp == DRV_ENC28J60_PHY_OP_READ_START)
       {
           phyOpDcpt.currOp = (uint16_t)DRV_ENC28J60_PHY_OP_READ;
       }
       else
       {    // invalid
           phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_IDLE;
           return DRV_ENC28J60_PHY_RES_OP_ERROR; 
       }
    }
    else
    {   // already mid operation
        if(phyOpDcpt.currOp != (uint16_t)currOp || phyOpDcpt.currReg != (uint16_t)reg)
        {
            return DRV_ENC28J60_PHY_RES_OP_ERROR; 
        }
    }
    
    
    // assume everything OK
    phyRes = DRV_ENC28J60_PHY_RES_PENDING; 
    switch(phyOpDcpt.currState)
    {
        case (uint16_t)DRV_OP_STATE_START_MIREGADDR:
            phyReg.value = 0;
            phyReg.miregadr.MIREGADR = (uint8_t)reg;

            if((phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrWrite(pDrvInstance, DRV_ENC28J60_SFR_MIREGADR, phyReg, false)) != 0)
            {   // success
                phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_WAIT_MIREGADDR;
            }
            else
            {
                phyOpDcpt.totRetryCnt++;
                if(++phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            // else retry is good
            break;

        case (uint16_t)DRV_OP_STATE_WAIT_MIREGADDR:
            busRes = DRV_ENC28J60_SPI_OperationResult(pDrvInstance, phyOpDcpt.currOpDcpt, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            }
            else
            {
                // OK
            }

            // done
            phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_START_MIIRD_MIWR;
            phyOpDcpt.retryCount = 0;
            break;

        case (uint16_t)DRV_OP_STATE_START_MIIRD_MIWR:
            if(phyOpDcpt.currOp == (uint16_t)DRV_ENC28J60_PHY_OP_READ)
            {   // advance: set the MICMD.MIIRD bit
                phyReg.value = 0;
                phyReg.micmd.MIIRD = 1;
                phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_MICMD, phyReg, false);
            }
            else
            {   // write
                phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrWrite16(pDrvInstance, DRV_ENC28J60_SFR_MIWRL, phyOpDcpt.wrValue, false);
            }

            if(phyOpDcpt.currOpDcpt != 0)
            {   // success
                phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_WAIT_MIIRD_MIWR;
            }
            else
            {
                phyOpDcpt.totRetryCnt++;
                if(++phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            break;

        case (uint16_t)DRV_OP_STATE_WAIT_MIIRD_MIWR:
            busRes = DRV_ENC28J60_SPI_OperationResult(pDrvInstance, phyOpDcpt.currOpDcpt, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            } 
            else
            {
                // OK
            }

            // done; advance: wait for the tick to pass
            phyOpDcpt.startTick = SYS_TMR_TickCountGet();
            phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_WAIT_TMO;
            break;

        case (uint16_t)DRV_OP_STATE_WAIT_TMO:
            if(SYS_TMR_TickCountGet() == phyOpDcpt.startTick)
            {   // wait some more
                break;
            }

            phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_POLL_BUSY;
            phyOpDcpt.retryCount = 0;
            // advance and poll MISTAT.BUSY
            break;

        case (uint16_t)DRV_OP_STATE_POLL_BUSY:
            if((phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrReadStart(pDrvInstance, DRV_ENC28J60_SFR_MISTAT, false)) != 0)
            {   // success
                phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_WAIT_BUSY;
            }
            else
            {   // failed
                phyOpDcpt.totRetryCnt++;
                if(++phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            break;

        case (uint16_t)DRV_OP_STATE_WAIT_BUSY:
            phyReg.value = 0;
            busRes = DRV_ENC28J60_SPI_SfrReadResult(pDrvInstance, phyOpDcpt.currOpDcpt, &phyReg, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            } 
            else
            {
                // OK
            }
            // have a read MISTAT reg
            if(phyReg.mistat.BUSY != 0U)
            {   // not ready, wait some more
                phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_POLL_BUSY;
                break;
            }
            
            // BUSY is cleared; operation complete
            if(phyOpDcpt.currOp == (uint16_t)DRV_ENC28J60_PHY_OP_WRITE)
            {   // we're done
                phyRes = DRV_ENC28J60_PHY_RES_OK; 
                break;
            }

            // more ops for read...advance: clear the MICMD.MIIRD bit
            phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_START_MICMD;
            phyOpDcpt.retryCount = 0;
            break;

        case (uint16_t)DRV_OP_STATE_START_MICMD:
            phyReg.value = 0;
            phyReg.micmd.MIIRD = 1;
            if((phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrBitClear(pDrvInstance, DRV_ENC28J60_SFR_MICMD, phyReg, false)) != 0)
            {   // success
                phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_WAIT_MICMD;
            }
            else
            {
                phyOpDcpt.totRetryCnt++;
                if(++phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            break;

        case (uint16_t)DRV_OP_STATE_WAIT_MICMD:
            busRes = DRV_ENC28J60_SPI_OperationResult(pDrvInstance, phyOpDcpt.currOpDcpt, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            } 
            else
            {
                // OK
            }
            
            // done; advance: read the MIRDL/MIRDL registers
            phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_START_MIRD;
            phyOpDcpt.retryCount = 0;
            break;

        case (uint16_t)DRV_OP_STATE_START_MIRD:
            if((phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrRead16Start(pDrvInstance, DRV_ENC28J60_SFR_MIRDL, false)) != 0)
            {   // success
                phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_WAIT_MIRD;
            }
            else
            {
                phyOpDcpt.totRetryCnt++;
                if(++phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            break;

        case (uint16_t)DRV_OP_STATE_WAIT_MIRD:
            busRes = DRV_ENC28J60_SPI_SfrRead16Result(pDrvInstance, phyOpDcpt.currOpDcpt, &phyOpDcpt.rdValue, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            } 
            else
            {
                // OK
            }

            // done: successfully got the result in the rdValue
            phyRes = DRV_ENC28J60_PHY_RES_OK; 
            break;

        default:
            // do nothing
            break;

    }

    if(phyRes != DRV_ENC28J60_PHY_RES_PENDING)
    {   // operation ended, one way or another
        phyOpDcpt.currState = (uint16_t)DRV_OP_STATE_IDLE;
    }


    return phyRes;
}



// *****************************************************************************
/* Write Data

    Summary:
    Writes data to the ENC hardware

    Details:
    This function writes data to the ENC.

    Preconditions:
    The bus had to have been initialized first.  The parameters to this function
    are a little different than expected.  The TCP/IP packet segment is allocated
    with some space before the actual pointer in the packet.  For the PIC32
    Internal MAC this is two bytes.  This data is used by the MAC for its own
    purposes.  In the case of the ENC28J60 only 1 byte is needed for SPI.The buffer 
    that is passed in is the start of the data segment, and this function assumes 
    there is some allocated space before the pointer.  The datasize is the size of
    the data portion, not the total size of the buffer.

    Parameters:
        pDrvInstance �?? The driver instance
        reg �?? the register to write to
        buffer �?? the location of the buffer to write
        dataSize �?? the size of the data to write.

    Returns:
        0 �?? on error
        Valid handle �?? on success
*/

//    Returns TCPIP_MAC_RES result:
// TCPIP_MAC_RES_PENDING if retry needed
// TCPIP_MAC_RES_OK if operation went through
// TCPIP_MAC_RES_PACKET_ERR - rejected, cannot transmit such packet
TCPIP_MAC_RES DRV_ENC28J60_SPI_WritePacket(DRV_ENC28J60_DriverInfo *  pDrvInstance, TCPIP_MAC_PACKET* pTxPkt, uintptr_t* pHandle, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    DRV_ENC28J60_spiBusData* pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;
    uint8_t* pTxBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_OP_EXT_BUFF*   pExtBuff;
    uint16_t    pktCopySize;
    uint8_t*    pStartAdd;
    TCPIP_MAC_RES   res;
    uint16_t    pktLen = 0;
    int         nTxSegs = 0;
    TCPIP_MAC_DATA_SEGMENT* pSeg = pTxPkt != NULL ? pTxPkt->pDSeg : NULL;

    if(autoAck == false && pHandle == NULL)
    {   // a packet that has to be acknowledged needs a handle!
        return TCPIP_MAC_RES_OP_ERR;
    }

    if(pSeg == NULL)
    {   // cannot send this packet; 
        return TCPIP_MAC_RES_PACKET_ERR;
    }


    if((pOpDcpt = F_DRV_GetOpDcpt(pBusInfo, autoAck, 0U)) == NULL)
    {   // retry, too many ops ongoing
        return TCPIP_MAC_RES_PENDING;
    }

    if(pHandle != NULL)
    {
        *pHandle = 0;
    }

    // Calculate packet size and number of segments
    while(pSeg != NULL)
    {
        nTxSegs++;
        pktLen += pSeg->segLen;
        pSeg = pSeg->next;
    }

    pExtBuff = NULL;

    while(true)
    {
        if(nTxSegs == 1)
        {   // no need for internal copy; use the internal packet space 
            pTxBuff = pTxPkt->pDSeg->segLoad - 2;
        }
        else
        {   // packet needs to be copied into internal buffer
            if(pktLen > pBusInfo->extBuffSize)
            {   // cannot transmit such packet
                res = TCPIP_MAC_RES_PACKET_ERR;
                break;
            }

            pExtBuff = F_DRV_GetExtBuffer(pBusInfo);
            if(pExtBuff == NULL)
            {   // not enough buffers; retry later
                res = TCPIP_MAC_RES_PENDING;
                break;
            }
            pOpDcpt->extBuffer = pExtBuff;

            // linearize packet
            pTxBuff = pExtBuff->extBuffer;
            pStartAdd = pTxPkt->pDSeg->segLoad;
            pktCopySize = TCPIP_Helper_PacketCopy(pTxPkt, pTxBuff + 2, &pStartAdd, pktLen, false);
            DRV_ENC28J60_Assert(pktCopySize == pktLen, __func__, __LINE__);
        }

        // Put the Write Buffer Memory Command
        *pTxBuff = (uint8_t)DRV_ENC28J60_SPI_INST_WBM;
        *(pTxBuff +1) = 0;  // control/override byte: no override, use the MACON settings

        //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pTxBuff, pktLen + 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pTxBuff, (size_t)pktLen + 2UL, &spiHandle);
        if(spiHandle == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            res = TCPIP_MAC_RES_PENDING;
            break;
        }

        // ok, scheduled successfully
        F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
        if(autoAck == false)
        {
            *pHandle = (uintptr_t)pOpDcpt;
        }
        return TCPIP_MAC_RES_OK;
    }

    // failed somehow; release the buffer
    F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    return res;
}

// Read packet data
uintptr_t DRV_ENC28J60_SPI_ReadDataStart(DRV_ENC28J60_DriverInfo *  pDrvInstance, uint8_t* buffer, uint16_t dataSize, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t*    pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;

    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if((pOpDcpt = F_DRV_GetOpDcpt(pBusInfo, autoAck, 0U)) != NULL)
    {
        pWrBuff = pOpDcpt->opWrBuffer;

        *pWrBuff = (uint8_t)DRV_ENC28J60_SPI_INST_RBM;   

        DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, pWrBuff, 1UL, &buffer[-1], (size_t)(dataSize + 1UL), &spiHandle);
        if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            F_DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
            return (uintptr_t)pOpDcpt;
        }
        F_DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    }

    return 0;
}


