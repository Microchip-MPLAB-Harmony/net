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
/*****************************************************************************
 Copyright (C) 2015-2020 Microchip Technology Inc. and its subsidiaries.

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
    DRV_ENC28J60_PHY_OP_STATE_IDLE,             // no operation in progress
    DRV_ENC28J60_PHY_OP_STATE_START_MIREGADDR,  // start MIREGADR transaction
    DRV_ENC28J60_PHY_OP_STATE_WAIT_MIREGADDR,   // wait for MIREGADR completion
    DRV_ENC28J60_PHY_OP_STATE_START_MIIRD_MIWR,  // start MIRD/MIWRLH transaction
    DRV_ENC28J60_PHY_OP_STATE_WAIT_MIIRD_MIWR,  // wait for MIRD/MIWRLH completion
    DRV_ENC28J60_PHY_OP_STATE_WAIT_TMO,         // wait for 10.24us to pass
    DRV_ENC28J60_PHY_OP_STATE_POLL_BUSY,        // poll MISTAT.BUSY to clear itself
    DRV_ENC28J60_PHY_OP_STATE_WAIT_BUSY,        // wait for MISTAT.BUSY to clear itself
    DRV_ENC28J60_PHY_OP_STATE_START_MICMD,      // start MICMD.MIIRD transaction
    DRV_ENC28J60_PHY_OP_STATE_WAIT_MICMD,       // wait for MICMD.MIIRD completion - read op only
    DRV_ENC28J60_PHY_OP_STATE_START_MIRD,       // start MIRDL/H transaction
    DRV_ENC28J60_PHY_OP_STATE_WAIT_MIRD,        // wait for MIRDL/H completion - read op only
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

static DRV_ENC28J60_PHY_OP_DCPT _phyOpDcpt = { 0 };

static const DRV_ENC28J60_BusVTable drv_ENC28J60_spi_vtable =
{
    .fpOpenIf = DRV_ENC28J60_SPI_OpenInterface,
    .fpCloseIf = DRV_ENC28J60_SPI_CloseInterface,
    .fpOpResult = DRV_ENC28J60_SPI_OperationResult,
    .fpSfrWr = DRV_ENC28J60_SPI_SfrWrite,
    .fpSfrWr16 = DRV_ENC28J60_SPI_SfrWrite16,
    .fpSfrRdStart = DRV_ENC28J60_SPI_SfrReadStart,
    .fpSfrRdResult = DRV_ENC28J60_SPI_SfrReadResult,
    .fpSfrRd16Start = DRV_ENC28J60_SPI_SfrRead16Start,
    .fpSfrRd16Result = DRV_ENC28J60_SPI_SfrRead16Result,
    .fpSfrBitSet = DRV_ENC28J60_SPI_SfrBitSet,
    .fpSfrBitClr = DRV_ENC28J60_SPI_SfrBitClear,
    .fpSysRst = DRV_ENC28J60_SPI_SystemReset,
    .fpRxEnable = DRV_ENC28J60_SPI_EnableRX,
    .fpRxDisable = DRV_ENC28J60_SPI_DisableRX,
    .fpDecPktCnt = DRV_ENC28J60_SPI_DecrPktCtr,
    .fpIntEnable = DRV_ENC28J60_SPI_EnableInterrupts,
    .fpIntDisable = DRV_ENC28J60_SPI_DisableInterrupts,
    .fpFlowCtrlDisable = DRV_ENC28J60_SPI_FlowCtrlDisable,
    .fpFlowCtrlSingle = DRV_ENC28J60_SPI_FlowCtrlSingle,
    .fpFlowCtrlMult = DRV_ENC28J60_SPI_FlowCtrlMult,
    .fpFlowCtrlClr = DRV_ENC28J60_SPI_FlowCtrClear,
    .fpPhyWrStart = DRV_ENC28J60_SPI_PhyWriteStart,
    .fpPhyWr = DRV_ENC28J60_SPI_PhyWrite,
    .fpPhyRdStart = DRV_ENC28J60_SPI_PhyReadStart,
    .fpPhyRd = DRV_ENC28J60_SPI_PhyRead,
    .fpPktWr = DRV_ENC28J60_SPI_WritePacket,
    .fpDataRdStart = DRV_ENC28J60_SPI_ReadDataStart,
};

static DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyProcess(DRV_ENC28J60_DriverInfo* pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg, DRV_ENC28J60_PHY_OP currOp);
static void _DRV_ReleaseOpDcpt(DRV_ENC28J60_spiBusData* pBusData, DRV_ENC28J60_OP_DCPT* pOpDcpt);
static DRV_ENC28J60_BUS_RESULT _DRV_ENC28J60_OpStatusToResult(DRV_ENC28J60_OP_DCPT* pOpDcpt);

static void _DRV_AutoAcknowledge(DRV_ENC28J60_spiBusData* pBusData)
{
    DRV_ENC28J60_BUS_RESULT res;
    SINGLE_LIST             newAutoList;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;

    TCPIP_Helper_SingleListInitialize(&newAutoList);
    while( (pOpDcpt = (DRV_ENC28J60_OP_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pBusData->opListAutoAck)) != 0)
    {   // even if it's linked we process it separately: it won't be queried!
        if(pOpDcpt->spiHandle != 0)
        {   // if the handle is 0, it was just added to the list
            res = _DRV_ENC28J60_OpStatusToResult(pOpDcpt);
            if(res != DRV_ENC28J60_BR_PENDING)
            {   // completed
                _DRV_ReleaseOpDcpt(pBusData, pOpDcpt);
                continue;
            }
        }

        // still busy or doesn't have a buffer
        TCPIP_Helper_SingleListTailAdd(&newAutoList, (SGL_LIST_NODE*)pOpDcpt);
    }
    TCPIP_Helper_SingleListAppend(&pBusData->opListAutoAck, &newAutoList);

}

static  DRV_ENC28J60_OP_DCPT* _DRV_GetOpDcpt(DRV_ENC28J60_spiBusData* pBusData, bool autoAck, DRV_ENC28J60_OP_FLAGS opFlags)
{
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;

    // check first if we need to auto acknowledge
    _DRV_AutoAcknowledge(pBusData);


    // grab a free node
    pOpDcpt = (DRV_ENC28J60_OP_DCPT*)TCPIP_Helper_SingleListHeadRemove(&pBusData->opFreeList);
    
    if(pOpDcpt)
    {
        pOpDcpt->next = 0;  // show it's not in any list;
        pOpDcpt->link = 0;  // show it's not linked;
        pOpDcpt->spiHandle = 0;
        pOpDcpt->extBuffer = 0;
        pOpDcpt->opStatus = DRV_ENC28J60_OP_STAT_BUSY;
        pOpDcpt->opFlags = autoAck ? opFlags | DRV_ENC28J60_OP_FLAG_AUTO_ACK : opFlags;
        pBusData->opDcptOkCnt++;
    }
    else
    {
        pBusData->opDcptFailCnt++;
    }
    return pOpDcpt;
} 

static __inline__ DRV_ENC28J60_OP_EXT_BUFF* __attribute__((always_inline)) _DRV_GetExtBuffer(DRV_ENC28J60_spiBusData* pBusData)
{
    DRV_ENC28J60_OP_EXT_BUFF* pExtBuff = (DRV_ENC28J60_OP_EXT_BUFF*)TCPIP_Helper_SingleListHeadRemove(&pBusData->extBuffFreeList);

    if(pExtBuff)
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
static void _DRV_ReleaseOpDcpt(DRV_ENC28J60_spiBusData* pBusData, DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    if(pOpDcpt->extBuffer)
    {
        TCPIP_Helper_SingleListTailAdd(&pBusData->extBuffFreeList, (SGL_LIST_NODE*)pOpDcpt->extBuffer);
        pOpDcpt->extBuffer = 0;
    }

    TCPIP_Helper_SingleListTailAdd(&pBusData->opFreeList, (SGL_LIST_NODE*)pOpDcpt);
    pOpDcpt->opStatus = DRV_ENC28J60_OP_STAT_IDLE;  // mark it as done
}

static void _DRV_ReleaseOpDcptEx(DRV_ENC28J60_spiBusData* pBusData, DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    if(pOpDcpt->extBuffer)
    {
        TCPIP_Helper_SingleListTailAdd(&pBusData->extBuffFreeList, (SGL_LIST_NODE*)pOpDcpt->extBuffer);
        pOpDcpt->extBuffer = 0;
    }

    TCPIP_Helper_SingleListTailAdd(&pBusData->opFreeList, (SGL_LIST_NODE*)pOpDcpt);
    pOpDcpt->opStatus = DRV_ENC28J60_OP_STAT_IDLE;  // mark it as done
}

static DRV_ENC28J60_BUS_RESULT _DRV_ENC28J60_OpStatusToResult(DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    DRV_SPI_TRANSFER_EVENT spiEv = DRV_SPI_TransferStatusGet (pOpDcpt->spiHandle);
    if((spiEv == DRV_SPI_TRANSFER_EVENT_COMPLETE) || (spiEv == DRV_SPI_TRANSFER_EVENT_HANDLE_EXPIRED))
    {   // success
        pOpDcpt->opStatus = DRV_ENC28J60_OP_STAT_SUCCESS;
        return DRV_ENC28J60_BR_SUCCESS;
    }
    else if(spiEv == DRV_SPI_TRANSFER_EVENT_ERROR)
    {
        pOpDcpt->opStatus = DRV_ENC28J60_OP_STAT_ERROR;
        return DRV_ENC28J60_BR_ERROR;
    }
    else
    {   // still busy
        DRV_ENC28J60_Assert(pOpDcpt->opStatus == DRV_ENC28J60_OP_STAT_BUSY, __func__, __LINE__);
        return DRV_ENC28J60_BR_PENDING;
    }
}

static DRV_ENC28J60_BUS_RESULT _DRV_ENC28J60_LinkedOpStatus(DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    DRV_ENC28J60_BUS_RESULT res;
    bool    isError;


    isError = false;
    for( ; pOpDcpt != 0; pOpDcpt = pOpDcpt->link)
    {
        res = _DRV_ENC28J60_OpStatusToResult(pOpDcpt);
        if(res == DRV_ENC28J60_BR_PENDING)
        {   // not all descriptors ready
            return DRV_ENC28J60_BR_PENDING;
        }
        else if(res == DRV_ENC28J60_BR_ERROR)
        {
            isError = true;
        }
    }

    return isError ? DRV_ENC28J60_BR_ERROR : DRV_ENC28J60_BR_SUCCESS;
}

static void _DRV_ReleaseLinkedOpDcpt(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_OP_DCPT* pOpDcpt)
{
    DRV_ENC28J60_OP_DCPT*   pDcpt;

    for(; pOpDcpt != 0; pOpDcpt = pDcpt)
    {
        pDcpt = pOpDcpt->link;
        _DRV_ReleaseOpDcptEx((DRV_ENC28J60_spiBusData *)pDrvInstance->busData, pOpDcpt);
    }

}

/*static void DRV_ENC28J60_SPI_OperationStarting (DRV_SPI_TRANSFER_EVENT event, DRV_SPI_TRANSFER_HANDLE bufferHandle, void * context )
{
    DRV_ENC28J60_DriverInfo*  pDrvInstance = (DRV_ENC28J60_DriverInfo*)context;
    PLIB_PORTS_PinClear(pDrvInstance->drvCfg.spiSSPortModule, pDrvInstance->drvCfg.spiSSPortChannel, pDrvInstance->drvCfg.spiSSPortPin);
}

// Note: each descriptor is released by itself when autoAck and added to the free list;
// the link between multiple linked descriptors should not be needed anymore
// because the autoAck operation cannot ask for status completion!
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
static void _DRV_OpSetSpiHandle(DRV_ENC28J60_spiBusData* pBusData, DRV_ENC28J60_OP_DCPT* pOpDcpt, DRV_SPI_TRANSFER_HANDLE spiHandle, bool autoAck)
{
    DRV_ENC28J60_Assert(spiHandle != 0 && pOpDcpt->spiHandle == 0, __func__, __LINE__);
    pOpDcpt->spiHandle = spiHandle;
    // internal ops have autoAck: DoBank!
    if(autoAck)
    {
        TCPIP_Helper_SingleListTailAdd(&pBusData->opListAutoAck, (SGL_LIST_NODE*)pOpDcpt);
    }
}

static DRV_ENC28J60_OP_DCPT* _DRV_OpHandleFromUserHandle(uintptr_t  handle, bool isLinked)
{
    bool cond;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt = (DRV_ENC28J60_OP_DCPT*)handle;

    if(isLinked)
    {   // possible that no all ops are done; but at least one should be busy
        cond = (pOpDcpt != 0 && pOpDcpt->link != 0 && (pOpDcpt->opStatus == DRV_ENC28J60_OP_STAT_BUSY || pOpDcpt->link->opStatus == DRV_ENC28J60_OP_STAT_BUSY));
        // for now linked nodes don't support autoAck
        if(pOpDcpt->next != 0)
        {
            cond = 0;
        }
    }
    else
    {
        cond = (pOpDcpt != 0 && pOpDcpt->opStatus == DRV_ENC28J60_OP_STAT_BUSY);
    }

    DRV_ENC28J60_Assert(cond, __func__, __LINE__);

    return cond ? pOpDcpt : 0;
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
    if (pBusData == 0)
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
    for(ix = 0; ix < sizeof(pBusData->opArray) / sizeof(*pBusData->opArray); ix++, pOpDcpt++)
    {
        pOpDcpt->pDrvInstance = pDrvInstance;
        TCPIP_Helper_SingleListTailAdd(&pBusData->opFreeList, (SGL_LIST_NODE*)pOpDcpt);
    }
    pExtBuff =  pBusData->extBuffArray;
    for(ix = 0; ix < sizeof(pBusData->extBuffArray) / sizeof(*pBusData->extBuffArray); ix++, pExtBuff++)
    {
        TCPIP_Helper_SingleListTailAdd(&pBusData->extBuffFreeList, (SGL_LIST_NODE*)pExtBuff);
    }
    pBusData->extBuffSize = sizeof(pExtBuff->extBuffer) - DRV_ENC28J60_OP_EXT_BUFFER_EXTRA_SIZE; 

    memset(&_phyOpDcpt, 0, sizeof(_phyOpDcpt));

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

        (*pDrvInstance->stackCfg.freeF)(pDrvInstance->stackCfg.memH, pDrvInstance->busData);
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
        pDrvInstance â?? The driver instance

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
      
    DRV_SPI_TRANSFER_SETUP spiClientInfo= {    
        .chipSelect = pDrvInstance->drvCfg.spiSetup.chipSelect,//SYS_PORT_PIN_RD9,
    };
    DRV_SPI_TransferSetup (pBusInfo->clientHandle, &spiClientInfo);
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
        pDrvInstance â?? The driver instance

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
        pDrvInstance â?? The driver instance
        handle - Handle created by the operation.

    Returns
        DRV_ENC28J60_BR_SUCCESS â?? if the operation was successful
        DRV_ENC28J60_BR_PENDING â?? if the operation is still pending
        DRV_ENC28J60_BR_ERROR â?? if there was an error in the operation

   Remarks:
        Supports linked descriptors
*/
DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_OperationResult( DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, bool ack )
{
    DRV_ENC28J60_BUS_RESULT res;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt = _DRV_OpHandleFromUserHandle(handle, false);

    if(pOpDcpt == 0)
    {   // no such descriptor
        return DRV_ENC28J60_BR_INVALID;
    }

    res = _DRV_ENC28J60_LinkedOpStatus(pOpDcpt);

    if(res != DRV_ENC28J60_BR_PENDING && ack)
    {   // completed
        _DRV_ReleaseLinkedOpDcpt(pDrvInstance, pOpDcpt);
    }

    return res;
}

// selects a new register bank
// returns true if succes or fail and needs retry
static bool _DRV_ENC28J60_DoBankSet(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP sfr)
{
    uint8_t     sfrBank;
    bool        success;
    DRV_ENC28J60_RegUnion regEcon1;

    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;


    sfrBank =  sfr / 0x20;   // ENCJ60 bank size
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
            success = DRV_ENC28J60_SPI_SfrBitClear(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0;
            break;
            
        case 1:
            // clear both banks
            if(DRV_ENC28J60_SPI_SfrBitClear(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0)
            {
                pBusInfo->currentBank = 0;  // just in case the next op fails
                regEcon1.value = 1;     // set bank 1
                success = DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0;
            }
            break;

        case 2:
            // clear both banks
            if(DRV_ENC28J60_SPI_SfrBitClear(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0)
            {
                pBusInfo->currentBank = 0;  // just in case the next op fails
                regEcon1.value = 2;     // set bank 2
                success =  DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0;
            }

            break;

        case 3:
            // set both banks
            success = DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, true) != 0;
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
        pDrvInstance â?? The driver instance
        reg â?? The Special Function Register to write to.
        Value â?? the value to write into the register

    Returns:
        NULL â?? On Error
        Valid Handle â?? on success
*/

uintptr_t DRV_ENC28J60_SPI_SfrWrite(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfr, DRV_ENC28J60_RegUnion  regValue, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t*                pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if(_DRV_ENC28J60_DoBankSet(pDrvInstance, sfr))
    {   // Put in the data
        if((pOpDcpt = _DRV_GetOpDcpt(pBusInfo, autoAck, 0)) != 0)
        {   
            pWrBuff = pOpDcpt->opWrBuffer;
            *pWrBuff = DRV_ENC28J60_SPI_INST_WCR | (sfr & 0x1f);
            *(pWrBuff + 1) = regValue.value;
            //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
            DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff, 2, &spiHandle);

            
            if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
            {
                _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
                return (uintptr_t)pOpDcpt;
            }
            // couldn't launch the SPI op
            _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
        }
    }

    return 0;
}

// try a 16 bit operation as 2 independent 8 bit operations
uintptr_t DRV_ENC28J60_SPI_SfrWrite16(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfrLow, uint16_t regValue, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle1, spiHandle2;
    uint8_t                 *pWrBuff1, *pWrBuff2;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt1 = 0;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt2 = 0;
    DRV_ENC28J60_spiBusData *pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if(_DRV_ENC28J60_DoBankSet(pDrvInstance, sfrLow))
    {   // Put in the data
        pOpDcpt1 = _DRV_GetOpDcpt(pBusInfo, autoAck, DRV_ENC28J60_OP_FLAG_LINKED);
        pOpDcpt2 = _DRV_GetOpDcpt(pBusInfo, autoAck, DRV_ENC28J60_OP_FLAG_LINKED);

        if(pOpDcpt1 && pOpDcpt2)
        {   // got both operations
            pWrBuff1 = pOpDcpt1->opWrBuffer;
            pWrBuff2 = pOpDcpt2->opWrBuffer;
            *pWrBuff1 = DRV_ENC28J60_SPI_INST_WCR | (sfrLow & 0x1f);
            *(pWrBuff1 + 1) = (uint8_t)regValue;
            *pWrBuff2 = DRV_ENC28J60_SPI_INST_WCR | ((sfrLow + 1) & 0x1f);
            *(pWrBuff2 + 1) = (uint8_t)(regValue >> 8);
            
            //spiHandle1 = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff1, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
            DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff1, 2, &spiHandle1);
            if(spiHandle1 != DRV_SPI_TRANSFER_HANDLE_INVALID)
            {
                //spiHandle2 = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff2, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
                DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff2, 2, &spiHandle2);
                if(spiHandle2 != DRV_SPI_TRANSFER_HANDLE_INVALID)
                {   // success
                    _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt1, spiHandle1, autoAck);
                    _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt2, spiHandle2, autoAck);
                    // link the descriptors
                    pOpDcpt1->link = pOpDcpt2;
                    return (uintptr_t)pOpDcpt1;
                }
                // couldn't launch the 2nd SPI op and the 1st one it's on its way
                // that's why this approach is better to be avoided..
                // mark it for auto deletion
                _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt1, spiHandle1, true);
                pOpDcpt1->opFlags |= DRV_ENC28J60_OP_FLAG_AUTO_ACK;
                pOpDcpt1->opFlags &= ~DRV_ENC28J60_OP_FLAG_LINKED;  // not linked anymore
                // make sure we don't release it now
                pOpDcpt1 = 0;
            }
        }
    }

    if(pOpDcpt1)
    {
        _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt1);
    }
    if(pOpDcpt2)
    {
        _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt2);
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

    if(!_DRV_ENC28J60_DoBankSet(pDrvInstance, sfr))
    {
        return 0;
    }

    if((pOpDcpt = _DRV_GetOpDcpt(pBusInfo, autoAck, 0)) != 0)
    {
        pWrBuff = pOpDcpt->opWrBuffer;
        *pWrBuff = DRV_ENC28J60_SPI_INST_RCR | (sfr & 0x1f);	

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
            _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
            return (uintptr_t)pOpDcpt;
        }

        _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    }

    return 0;
}

uintptr_t DRV_ENC28J60_SPI_SfrRead16Start(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  sfrLow, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle1, spiHandle2;
    uint8_t                 *pWrBuff1, *pWrBuff2;
    uint8_t                 *pRdBuff1, *pRdBuff2;
    size_t                  txferLen;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt1 = 0;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt2 = 0;

    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if(!_DRV_ENC28J60_DoBankSet(pDrvInstance, sfrLow))
    {
        return 0;
    }

    pOpDcpt1 = _DRV_GetOpDcpt(pBusInfo, autoAck, DRV_ENC28J60_OP_FLAG_LINKED);
    pOpDcpt2 = _DRV_GetOpDcpt(pBusInfo, autoAck, DRV_ENC28J60_OP_FLAG_LINKED);

    if(pOpDcpt1 && pOpDcpt2)
    {   // got both operations
        pWrBuff1 = pOpDcpt1->opWrBuffer;
        pWrBuff2 = pOpDcpt2->opWrBuffer;

        *pWrBuff1 = DRV_ENC28J60_SPI_INST_RCR | (sfrLow & 0x1f);	
        *pWrBuff2 = DRV_ENC28J60_SPI_INST_RCR | ((sfrLow + 1) & 0x1f);	

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
                _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt1, spiHandle1, autoAck);
                _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt2, spiHandle2, autoAck);
                // link the descriptors
                pOpDcpt1->link = pOpDcpt2;
                return (uintptr_t)pOpDcpt1;
            }
            // couldn't launch the 2nd SPI op and the 1st one it's on its way
            // that's why this approach is better to be avoided..
            // mark it for auto deletion
            _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt1, spiHandle1, true);
            pOpDcpt1->opFlags |= DRV_ENC28J60_OP_FLAG_AUTO_ACK;
            pOpDcpt1->opFlags &= ~DRV_ENC28J60_OP_FLAG_LINKED;  // not linked anymore
            // make sure we don't release it now
            pOpDcpt1 = 0;
        }
    }

    if(pOpDcpt1)
    {
        _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt1);
    }
    if(pOpDcpt2)
    {
        _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt2);
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
        pDrvInstance â?? The driver instance
        handle â?? the handle from the read start operation
        value â?? where to put the results of the operation

    Returns
        DRV_ENC28J60_BR_SUCCESS â?? if the operation was successful
        DRV_ENC28J60_BR_PENDING â?? if the operation is still pending
        DRV_ENC28J60_BR_ERROR â?? if there was an error in the operation
*/
DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_SfrReadResult(DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, DRV_ENC28J60_RegUnion * regValue, bool ack)
{
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_BUS_RESULT res = DRV_ENC28J60_BR_INVALID;

    pOpDcpt = _DRV_OpHandleFromUserHandle(handle, false);

    // make sure this is not an 16 bit op!
    if(pOpDcpt && pOpDcpt->link == 0)
    {
        res = _DRV_ENC28J60_LinkedOpStatus(pOpDcpt);

        if (res == DRV_ENC28J60_BR_SUCCESS)
        {
            regValue->value = pOpDcpt->opRdBuffer[2];
        }

        if(res != DRV_ENC28J60_BR_PENDING && ack)
        {
            _DRV_ReleaseLinkedOpDcpt(pDrvInstance, pOpDcpt);
        }
    }

    return res;
}

DRV_ENC28J60_BUS_RESULT DRV_ENC28J60_SPI_SfrRead16Result(DRV_ENC28J60_DriverInfo *  pDrvInstance, uintptr_t  handle, uint16_t* value, bool ack)
{
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_BUS_RESULT res = DRV_ENC28J60_BR_INVALID;

    pOpDcpt = _DRV_OpHandleFromUserHandle(handle, true);

    // make sure this is an 16 bit op!
    if(pOpDcpt && pOpDcpt->link)
    {
        res = _DRV_ENC28J60_LinkedOpStatus(pOpDcpt);

        if (res == DRV_ENC28J60_BR_SUCCESS)
        {
            *value = pOpDcpt->opRdBuffer[2] | ((uint16_t)pOpDcpt->link->opRdBuffer[2] << 8);
        }

        if(res != DRV_ENC28J60_BR_PENDING && ack)
        {   // completed
            _DRV_ReleaseLinkedOpDcpt(pDrvInstance, pOpDcpt);
        }
    }
    return res;
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
        pDrvInstance â?? The driver instance
        reg â?? The Special Function Register to write to.
        Value â?? The bits to set in the register.

    Returns
        NULL â?? On Error
        Valid Handle â?? on success
*/
uintptr_t DRV_ENC28J60_SPI_SfrBitSet(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  regValue, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t *pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    //Select and Set the bank address for the SFR 
    if(_DRV_ENC28J60_DoBankSet(pDrvInstance, reg))
    {
        if((pOpDcpt = _DRV_GetOpDcpt(pBusInfo, autoAck, 0)) != 0)
        {
            pWrBuff = pOpDcpt->opWrBuffer;

            // Put the Bit Field Set Command and SFR Adr 
            *pWrBuff = DRV_ENC28J60_SPI_INST_BFS | (reg & 0x1f);
            // Put in the bit information to SET
            *(pWrBuff + 1) = regValue.value & 0xff;

            //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
            DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff, 2, &spiHandle);
        
            if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
            {
                _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
                return (uintptr_t)pOpDcpt;
            }

            _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
        }
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
        pDrvInstance â?? The driver instance
        reg â?? The Special Function Register to write to.
        Value â?? The bits to clear in the register.

    Returns
        NULL â?? On Error
        Valid Handle â?? on success
*/
uintptr_t DRV_ENC28J60_SPI_SfrBitClear(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_SFR_MAP  reg, DRV_ENC28J60_RegUnion  regValue, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t*    pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;
    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;


    if(_DRV_ENC28J60_DoBankSet(pDrvInstance, reg))
    {
        if((pOpDcpt = _DRV_GetOpDcpt(pBusInfo, autoAck, 0)) != 0)
        {
            pWrBuff = pOpDcpt->opWrBuffer;

            // Put the Bit Field Set Command and SFR Adr 
            *pWrBuff = DRV_ENC28J60_SPI_INST_BFC | (reg & 0x1f);
            // Put in the bit information to CLR
            *(pWrBuff + 1) = regValue.value & 0xff;

            //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff, 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
            DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff, 2, &spiHandle);
            if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
            {
                _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
                return (uintptr_t)pOpDcpt;
            }

            _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
        }
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

    if((pOpDcpt = _DRV_GetOpDcpt(pBusInfo, autoAck, 0)) != 0)
    {
        pWrBuff = pOpDcpt->opWrBuffer;

        *pWrBuff = DRV_ENC28J60_SPI_INST_SRC;
        //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pWrBuff, 1, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pWrBuff, 1, &spiHandle);
        if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
            return (uintptr_t)pOpDcpt;
        }
        _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
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
        pDrvInstance â?? The driver instance

    Returns
        NULL â?? On Error
        Valid Handle â?? on success
*/
uintptr_t DRV_ENC28J60_SPI_EnableRX(DRV_ENC28J60_DriverInfo *  pDrvInstance, bool autoAck )
{
    DRV_ENC28J60_RegUnion regEcon1 = {0};

    regEcon1.econ1.RXEN = 1;

    return DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, autoAck);
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

    return DRV_ENC28J60_SPI_SfrBitClear(pDrvInstance, DRV_ENC28J60_SFR_ECON1, regEcon1, autoAck);
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

    return DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_ECON2, regEcon2, autoAck);
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
DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyWriteStart(struct _DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP  reg, uint16_t regValue)
{
    if(_phyOpDcpt.currState != DRV_ENC28J60_PHY_OP_STATE_IDLE)
    {
        return DRV_ENC28J60_PHY_RES_BUSY; 
    }

    _phyOpDcpt.wrValue = regValue;

    return DRV_ENC28J60_SPI_PhyProcess(pDrvInstance, reg, DRV_ENC28J60_PHY_OP_WRITE_START);

}

// advances the write state machine
DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyWrite(DRV_ENC28J60_DriverInfo *  pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg)
{
    return DRV_ENC28J60_SPI_PhyProcess(pDrvInstance, reg, DRV_ENC28J60_PHY_OP_WRITE);
}

DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyReadStart(DRV_ENC28J60_DriverInfo* pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg)
{
    if(_phyOpDcpt.currState != DRV_ENC28J60_PHY_OP_STATE_IDLE)
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
        *phyRegValue = _phyOpDcpt.rdValue;
    }

    return phyRes;
}


static DRV_ENC28J60_PHY_RES DRV_ENC28J60_SPI_PhyProcess(DRV_ENC28J60_DriverInfo* pDrvInstance, DRV_ENC28J60_PHY_SFR_MAP reg, DRV_ENC28J60_PHY_OP currOp)
{
    DRV_ENC28J60_PHY_RES    phyRes;
    DRV_ENC28J60_BUS_RESULT busRes;
    DRV_ENC28J60_RegUnion   phyReg;
    
    if(_phyOpDcpt.currState == DRV_ENC28J60_PHY_OP_STATE_IDLE)
    {   // starting an operation

        _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_START_MIREGADDR;
        _phyOpDcpt.currReg = reg;
        _phyOpDcpt.retryCount = 0;
       if(currOp == DRV_ENC28J60_PHY_OP_WRITE_START)
       {
           _phyOpDcpt.currOp = DRV_ENC28J60_PHY_OP_WRITE;
       }
       else if(currOp == DRV_ENC28J60_PHY_OP_READ_START)
       {
           _phyOpDcpt.currOp = DRV_ENC28J60_PHY_OP_READ;
       }
       else
       {    // invalid
           _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_IDLE;
           return DRV_ENC28J60_PHY_RES_OP_ERROR; 
       }
    }
    else
    {   // already mid operation
        if(_phyOpDcpt.currOp != currOp || _phyOpDcpt.currReg != reg)
        {
            return DRV_ENC28J60_PHY_RES_OP_ERROR; 
        }
    }
    
    
    // assume everything OK
    phyRes = DRV_ENC28J60_PHY_RES_PENDING; 
    switch(_phyOpDcpt.currState)
    {
        case DRV_ENC28J60_PHY_OP_STATE_START_MIREGADDR:
            phyReg.value = 0;
            phyReg.miregadr.MIREGADR = reg;

            if((_phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrWrite(pDrvInstance, DRV_ENC28J60_SFR_MIREGADR, phyReg, false)) != 0)
            {   // success
                _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_WAIT_MIREGADDR;
            }
            else
            {
                _phyOpDcpt.totRetryCnt++;
                if(++_phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            // else retry is good
            break;

        case DRV_ENC28J60_PHY_OP_STATE_WAIT_MIREGADDR:
            busRes = DRV_ENC28J60_SPI_OperationResult(pDrvInstance, _phyOpDcpt.currOpDcpt, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            }

            // done
            _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_START_MIIRD_MIWR;
            _phyOpDcpt.retryCount = 0;
            // no break;

        case DRV_ENC28J60_PHY_OP_STATE_START_MIIRD_MIWR:
            if(_phyOpDcpt.currOp == DRV_ENC28J60_PHY_OP_READ)
            {   // advance: set the MICMD.MIIRD bit
                phyReg.value = 0;
                phyReg.micmd.MIIRD = 1;
                _phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrBitSet(pDrvInstance, DRV_ENC28J60_SFR_MICMD, phyReg, false);
            }
            else
            {   // write
                _phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrWrite16(pDrvInstance, DRV_ENC28J60_SFR_MIWRL, _phyOpDcpt.wrValue, false);
            }

            if(_phyOpDcpt.currOpDcpt != 0)
            {   // success
                _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_WAIT_MIIRD_MIWR;
            }
            else
            {
                _phyOpDcpt.totRetryCnt++;
                if(++_phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            break;

        case DRV_ENC28J60_PHY_OP_STATE_WAIT_MIIRD_MIWR:
            busRes = DRV_ENC28J60_SPI_OperationResult(pDrvInstance, _phyOpDcpt.currOpDcpt, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            } 

            // done; advance: wait for the tick to pass
            _phyOpDcpt.startTick = SYS_TMR_TickCountGet();
            _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_WAIT_TMO;
            break;

        case DRV_ENC28J60_PHY_OP_STATE_WAIT_TMO:
            if(SYS_TMR_TickCountGet() == _phyOpDcpt.startTick)
            {   // wait some more
                break;
            }

            _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_POLL_BUSY;
            _phyOpDcpt.retryCount = 0;
            // advance and poll MISTAT.BUSY
            // no break;

        case DRV_ENC28J60_PHY_OP_STATE_POLL_BUSY:
            if((_phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrReadStart(pDrvInstance, DRV_ENC28J60_SFR_MISTAT, false)) != 0)
            {   // success
                _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_WAIT_BUSY;
            }
            else
            {   // failed
                _phyOpDcpt.totRetryCnt++;
                if(++_phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            break;

        case DRV_ENC28J60_PHY_OP_STATE_WAIT_BUSY:
            busRes = DRV_ENC28J60_SPI_SfrReadResult(pDrvInstance, _phyOpDcpt.currOpDcpt, &phyReg, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            } 
            // have a read MISTAT reg
            if(phyReg.mistat.BUSY)
            {   // not ready, wait some more
                _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_POLL_BUSY;
                break;
            }
            
            // BUSY is cleared; operation complete
            if(_phyOpDcpt.currOp == DRV_ENC28J60_PHY_OP_WRITE)
            {   // we're done
                phyRes = DRV_ENC28J60_PHY_RES_OK; 
                break;
            }

            // more ops for read...advance: clear the MICMD.MIIRD bit
            _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_START_MICMD;
            _phyOpDcpt.retryCount = 0;
            // no break

        case DRV_ENC28J60_PHY_OP_STATE_START_MICMD:
            phyReg.value = 0;
            phyReg.micmd.MIIRD = 1;
            if((_phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrBitClear(pDrvInstance, DRV_ENC28J60_SFR_MICMD, phyReg, false)) != 0)
            {   // success
                _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_WAIT_MICMD;
            }
            else
            {
                _phyOpDcpt.totRetryCnt++;
                if(++_phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            break;

        case DRV_ENC28J60_PHY_OP_STATE_WAIT_MICMD:
            busRes = DRV_ENC28J60_SPI_OperationResult(pDrvInstance, _phyOpDcpt.currOpDcpt, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            } 
            
            // done; advance: read the MIRDL/MIRDL registers
            _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_START_MIRD;
            _phyOpDcpt.retryCount = 0;
            // no break

        case DRV_ENC28J60_PHY_OP_STATE_START_MIRD:
            if((_phyOpDcpt.currOpDcpt = DRV_ENC28J60_SPI_SfrRead16Start(pDrvInstance, DRV_ENC28J60_SFR_MIRDL, false)) != 0)
            {   // success
                _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_WAIT_MIRD;
            }
            else
            {
                _phyOpDcpt.totRetryCnt++;
                if(++_phyOpDcpt.retryCount >= DRV_ENC28J60_PHY_OP_RETRIES)
                {   // exceeded retries 
                    phyRes = DRV_ENC28J60_PHY_RES_RESOURCE_ERROR;
                }
            }
            break;

        case DRV_ENC28J60_PHY_OP_STATE_WAIT_MIRD:
            busRes = DRV_ENC28J60_SPI_SfrRead16Result(pDrvInstance, _phyOpDcpt.currOpDcpt, &_phyOpDcpt.rdValue, true);
            if(busRes < 0)
            {
                phyRes = DRV_ENC28J60_PHY_RES_TXFER_ERROR; 
                break;
            }
            else if(busRes == DRV_ENC28J60_BR_PENDING)
            {
                break;
            } 

            // done: successfully got the result in the rdValue
            phyRes = DRV_ENC28J60_PHY_RES_OK; 
            break;

        default:
            break;

    }

    if(phyRes != DRV_ENC28J60_PHY_RES_PENDING)
    {   // operation ended, one way or another
        _phyOpDcpt.currState = DRV_ENC28J60_PHY_OP_STATE_IDLE;
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
        pDrvInstance â?? The driver instance
        reg â?? the register to write to
        buffer â?? the location of the buffer to write
        dataSize â?? the size of the data to write.

    Returns:
        0 â?? on error
        Valid handle â?? on success
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
    TCPIP_MAC_DATA_SEGMENT* pSeg = pTxPkt ? pTxPkt->pDSeg : 0;

    if(autoAck == false && pHandle == 0)
    {   // a packet that has to be acknowledged needs a handle!
        return TCPIP_MAC_RES_OP_ERR;
    }

    if(pSeg == 0)
    {   // cannot send this packet; 
        return TCPIP_MAC_RES_PACKET_ERR;
    }


    if((pOpDcpt = _DRV_GetOpDcpt(pBusInfo, autoAck, 0)) == 0)
    {   // retry, too many ops ongoing
        return TCPIP_MAC_RES_PENDING;
    }

    if(pHandle)
    {
        *pHandle = 0;
    }

    // Calculate packet size and number of segments
    while(pSeg != 0)
    {
        nTxSegs++;
        pktLen += pSeg->segLen;
        pSeg = pSeg->next;
    }

    pExtBuff = 0;

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

            pExtBuff = _DRV_GetExtBuffer(pBusInfo);
            if(pExtBuff == 0)
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
        *pTxBuff = DRV_ENC28J60_SPI_INST_WBM;
        *(pTxBuff +1) = 0;  // control/override byte: no override, use the MACON settings

        //spiHandle = DRV_SPI_BufferAddWrite(pBusInfo->clientHandle, pTxBuff, pktLen + 2, DRV_ENC28J60_SPI_Acknowledge, pDrvInstance);
        DRV_SPI_WriteTransferAdd(pBusInfo->clientHandle, pTxBuff, pktLen + 2, &spiHandle);
        if(spiHandle == DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            res = TCPIP_MAC_RES_PENDING;
            break;
        }

        // ok, scheduled successfully
        _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
        if(autoAck == false)
        {
            *pHandle = (uintptr_t)pOpDcpt;
        }
        return TCPIP_MAC_RES_OK;
    }

    // failed somehow; release the buffer
    _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    return res;
}

// Read packet data
uintptr_t DRV_ENC28J60_SPI_ReadDataStart(DRV_ENC28J60_DriverInfo *  pDrvInstance, uint8_t* buffer, uint16_t dataSize, bool autoAck)
{
    DRV_SPI_TRANSFER_HANDLE   spiHandle;
    uint8_t*    pWrBuff;
    DRV_ENC28J60_OP_DCPT*   pOpDcpt;

    DRV_ENC28J60_spiBusData * pBusInfo = (DRV_ENC28J60_spiBusData *)pDrvInstance->busData;

    if((pOpDcpt = _DRV_GetOpDcpt(pBusInfo, autoAck, 0)) != 0)
    {
        pWrBuff = pOpDcpt->opWrBuffer;

        *pWrBuff = DRV_ENC28J60_SPI_INST_RBM;	

        DRV_SPI_WriteReadTransferAdd(pBusInfo->clientHandle, pWrBuff, 1, &buffer[-1], dataSize + 1, &spiHandle);
        if(spiHandle != DRV_SPI_TRANSFER_HANDLE_INVALID)
        {
            _DRV_OpSetSpiHandle(pBusInfo, pOpDcpt, spiHandle, autoAck);
            return (uintptr_t)pOpDcpt;
        }
        _DRV_ReleaseOpDcpt(pBusInfo, pOpDcpt);
    }

    return 0;
}


