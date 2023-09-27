/*******************************************************************************
  Ethernet MIIM Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ethmiim.c

  Summary:
    Ethernet MIIM Device Driver Dynamic Implementation

  Description:
    The Ethernet MIIM device driver provides a simple interface to manage the
    Ethernet MIIM modules on Microchip microcontrollers.  This file Implements
    the core interface routines for the Ethernet MIIM driver.

    While building the driver from source, ALWAYS use this file in the build.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*
Copyright (C) 2013-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "driver/miim/src/drv_miim_local.h"
#include "drv_miim_device.h"

#include "system/sys_time_h2_adapter.h"
#include "system/debug/sys_debug.h"



// Local Definitions
//
static DRV_MIIM_OBJ              gDrvMIIMObj[DRV_MIIM_INSTANCES_NUMBER];

// local prototypes
// debug
#if ((DRV_MIIM_DEBUG_LEVEL & DRV_MIIM_DEBUG_MASK_BASIC) != 0)
volatile int _MIIMStayAssertLoop = 0; 
static void _MIIMAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("dMIIM Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_MIIMStayAssertLoop != 0);
    }
}

// a debug condition, not really assertion
volatile int _MIIMStayCondLoop = 0;
static void _MIIMDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("dMIIM Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_MIIMStayCondLoop != 0);
    }
}

#else
#define _MIIMAssertCond(cond, message, lineNo)
#define _MIIMDebugCond(cond, message, lineNo)
#endif  // (DRV_MIIM_DEBUG_LEVEL & DRV_MIIM_DEBUG_MASK_BASIC)


#if ((DRV_MIIM_DEBUG_LEVEL & DRV_MIIM_DEBUG_MASK_OPER) != 0)
static void _MIIM_Debug_Oper(DRV_MIIM_OP_DCPT* pOpDcpt, const char* msg)
{
    SYS_CONSOLE_PRINT("dMIIM oper %s - opType: %d, reg: %d, stat: %d, data: 0x%04x\r\n", msg, pOpDcpt->opType,  pOpDcpt->regIx, pOpDcpt->txferStat, pOpDcpt->opData);
}

#else
#define _MIIM_Debug_Oper(pOpDcpt, msg)
#endif  // ((DRV_MIIM_DEBUG_LEVEL & DRV_MIIM_DEBUG_MASK_OPER) != 0)

#if ((DRV_MIIM_DEBUG_LEVEL & DRV_MIIM_DEBUG_MASK_RW) != 0)
static void _MIIM_Debug_Write(DRV_MIIM_OP_DCPT* pOpDcpt)
{
    SYS_CONSOLE_PRINT("dMIIM write - reg: %d, add: 0x%04x, wdata: 0x%04x\r\n", pOpDcpt->regIx, pOpDcpt->phyAdd, pOpDcpt->opData);
}

static void _MIIM_Debug_Read(DRV_MIIM_OP_DCPT* pOpDcpt)
{
    SYS_CONSOLE_PRINT("dMIIM read - op reg: %d, add: 0x%04x\r\n", pOpDcpt->regIx, pOpDcpt->phyAdd);
}

static void _MIIM_Debug_ReadData(DRV_MIIM_OP_DCPT* pOpDcpt)
{
    SYS_CONSOLE_PRINT("dMIIM read data - op reg: %d, data: 0x%04x\r\n", pOpDcpt->regIx, pOpDcpt->opData);
}

#else
#define _MIIM_Debug_Write(pOpDcpt)
#define _MIIM_Debug_Read(pOpDcpt)
#define _MIIM_Debug_ReadData(pOpDcpt)
#endif  // ((DRV_MIIM_DEBUG_LEVEL & DRV_MIIM_DEBUG_MASK_RW) != 0)

#if ((DRV_MIIM_DEBUG_LEVEL & DRV_MIIM_DEBUG_MASK_EXT_SMI_RW) != 0)
static void _MIIM_Debug_ExtSMIWrite(DRV_MIIM_OP_DCPT* pOpDcpt, uint8_t phyAdd, uint8_t regVal, uint16_t wData, int seq)
{
    SYS_CONSOLE_PRINT("dMIIM Ext-SMI write - op reg: 0x%04x, add: 0x%04x, reg: 0x%04x, wdata: 0x%04x, seq: %d\r\n", pOpDcpt->regIx, phyAdd, regVal, wData, seq);
}

static void _MIIM_Debug_ExtSMIRead(DRV_MIIM_OP_DCPT* pOpDcpt, uint8_t phyAdd, uint8_t regVal, int seq)
{
    SYS_CONSOLE_PRINT("dMIIM Ext-SMI read - op reg: 0x%04x, add: 0x%04x, reg: 0x%04x, seq: %d\r\n", pOpDcpt->regIx, phyAdd, regVal, seq);
}

static void _MIIM_Debug_ExtSMIReadData(DRV_MIIM_OP_DCPT* pOpDcpt, int seq, uint16_t rdData)
{
    SYS_CONSOLE_PRINT("dMIIM Ext-SMI read data (%s): 0x%04x\r\n", seq == 0 ? "low" : "high", rdData);
}

#else
#define _MIIM_Debug_ExtSMIWrite(pOpDcpt, phyAdd, regVal, wData, seq)
#define _MIIM_Debug_ExtSMIRead(pOpDcpt, phyAdd, regVal, seq)
#define _MIIM_Debug_ExtSMIReadData(pOpDcpt, seq, rdData)
#endif  // ((DRV_MIIM_DEBUG_LEVEL & DRV_MIIM_DEBUG_MASK_EXT_SMI_RW) != 0)


static DRV_MIIM_OPERATION_HANDLE _DRV_MIIM_ScheduleOp(DRV_HANDLE handle, uint16_t regIx, uint16_t phyAdd, uint32_t opData, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult, DRV_MIIM_OP_TYPE opType);

static void _DRV_MIIM_ProcessOp( DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt);

static DRV_MIIM_REPORT_ACT _DRV_MIIM_ReportOp(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt);

static DRV_MIIM_RESULT _DRV_MIIM_OpResult(DRV_MIIM_OP_DCPT* pOpDcpt, bool scanAck);

static DRV_MIIM_OP_DCPT* _DRV_MIIM_GetOpDcpt(DRV_MIIM_CLIENT_DCPT* pClient, DRV_MIIM_OPERATION_HANDLE opHandle);

static void _DRV_MIIM_ReleaseOpDcpt(DRV_MIIM_OBJ* pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt, SINGLE_LIST* pRemList, DRV_MIIM_QUEUE_TYPE qType);

static DRV_MIIM_CLIENT_DCPT* _DRV_MIIM_ClientAllocate( DRV_MIIM_OBJ* pMiimObj, int* pCliIx);

static void _DRV_MIIM_ClientDeallocate( DRV_MIIM_CLIENT_DCPT* pClient);

static void _DRV_MIIM_ReadStartExt(DRV_MIIM_OBJ* pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt, int seq);

static void _DRV_MIIM_WriteDataExt(DRV_MIIM_OBJ* pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt, int seq);


static void _DRV_MIIM_PurgeClientOp(DRV_MIIM_CLIENT_DCPT* pClient);

// locks access to object lists and resources
// between user threads and task thread
static __inline__ void __attribute__((always_inline)) _DRV_MIIM_ObjLock(DRV_MIIM_OBJ* pMiimObj)
{
    (void)OSAL_SEM_Pend(&pMiimObj->objSem, OSAL_WAIT_FOREVER);
}

// unlocks access to object lists and resources
static __inline__ void __attribute__((always_inline)) _DRV_MIIM_ObjUnlock(DRV_MIIM_OBJ* pMiimObj)
{
    (void)OSAL_SEM_Post(&pMiimObj->objSem);
}


// returns a MIIM object from a SYS_MODULE_OBJ
static __inline__ DRV_MIIM_OBJ* __attribute__((always_inline)) _DRV_MIIM_GetObject(SYS_MODULE_OBJ object)
{
    DRV_MIIM_OBJ* pMiimObj;

    pMiimObj = (DRV_MIIM_OBJ*)object;
    
    if(pMiimObj != 0)
    {
        if(pMiimObj->objStatus == SYS_STATUS_READY && (pMiimObj->objFlags & DRV_MIIM_OBJ_FLAG_IN_USE) != 0)
        {   // minimal sanity check
            return pMiimObj;
        }
    }

    return 0;
}

// returns a MIIM object from a SYS_MODULE_OBJ
// and locks the object
static DRV_MIIM_OBJ* _DRV_MIIM_GetObjectAndLock(SYS_MODULE_OBJ object)
{
    DRV_MIIM_OBJ* pMiimObj = _DRV_MIIM_GetObject(object);

    if(pMiimObj != 0)
    {
        _DRV_MIIM_ObjLock(pMiimObj);
        return pMiimObj;
    }

    return 0;
}

// returns a client descriptor from DRV_HANDLE
// locks the corresponding parent object
static DRV_MIIM_CLIENT_DCPT* _DRV_MIIM_GetClientAndLock(DRV_HANDLE handle, bool lock)
{
    DRV_MIIM_OBJ* pMiimObj;
    DRV_MIIM_CLIENT_DCPT* pClient = (DRV_MIIM_CLIENT_DCPT*)handle;

    if(pClient != 0)
    {
        if(pClient->clientInUse != 0 && pClient->cliStatus == DRV_MIIM_CLIENT_STATUS_READY)
        {
            if(lock)
            {
                pMiimObj = _DRV_MIIM_GetObjectAndLock((SYS_MODULE_OBJ)pClient->parentObj);
            }
            else
            {
                pMiimObj = _DRV_MIIM_GetObject((SYS_MODULE_OBJ)pClient->parentObj);
            }

            if(pMiimObj != 0)
            {
                return pClient;
            }
        }
    }
    return 0;
}


static __inline__ void __attribute__((always_inline)) _DRV_MIIM_OpListAdd(SINGLE_LIST* pL, DRV_MIIM_OP_DCPT* pOpDcpt, DRV_MIIM_QUEUE_TYPE qType)
{
    Helper_SingleListTailAdd(pL, (SGL_LIST_NODE*)pOpDcpt);
    pOpDcpt->qType = qType;
}

static __inline__ DRV_MIIM_OP_DCPT* __attribute__((always_inline)) _DRV_MIIM_OpListRemove(SINGLE_LIST* pL, DRV_MIIM_QUEUE_TYPE qType)
{
    DRV_MIIM_OP_DCPT* pOpDcpt = (DRV_MIIM_OP_DCPT*)Helper_SingleListHeadRemove(pL);
    if(pOpDcpt)
    {
        _MIIMAssertCond(pOpDcpt->qType == qType, __func__, __LINE__);
    }
    return pOpDcpt;
}

// Extended SMI address from a register value
static __inline__ uint16_t __attribute__((always_inline)) _ExtSMI_Address(uint16_t regVal)
{
    return ((regVal >> 6) & 0x0f) | 0x10;
}

// Extended SMI register from a register value
static __inline__ uint16_t __attribute__((always_inline)) _ExtSMI_Register(uint16_t regVal, int seq)
{
    uint16_t reg = (regVal >> 1) & 0x1f;
    if(seq != 0)
    {
        reg |= 0x01;
    }
    return reg;
}

// Extended SMI 16 bit data from 32 bit data
static __inline__ uint16_t __attribute__((always_inline)) _ExtSMI_Data(uint32_t opData, int seq)
{
    uint16_t wData;
    if(seq == 0)
    {
        wData = (uint16_t)opData;
    }
    else
    {
        wData = (uint16_t)(opData >> 16);
    }
    return wData;
}

// processing function per MIIM operation table
static void MIIM_Process_Read(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt);
static void MIIM_Process_Write(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt);
static void MIIM_Process_Scan(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt);
static void MIIM_Process_ReadExt(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt);
static void MIIM_Process_WriteExt(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt);

// table with operations processing functions
static const DRV_MIIM_PROCESS_OP_FNC miim_process_op_tbl[] =
{
    MIIM_Process_Read,      // DRV_MIIM_OP_READ
    MIIM_Process_Write,     // DRV_MIIM_OP_WRITE
    MIIM_Process_Scan,      // DRV_MIIM_OP_SCAN
    MIIM_Process_ReadExt,   // DRV_MIIM_OP_READ_EXT
    MIIM_Process_WriteExt,  // DRV_MIIM_OP_WRITE_EXT
};




// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// The default basic MIIM driver object
//
const DRV_MIIM_OBJECT_BASE  DRV_MIIM_OBJECT_BASE_Default = 
{
    // system level object functions
    .DRV_MIIM_Initialize = DRV_MIIM_Initialize,
    .DRV_MIIM_Reinitialize = DRV_MIIM_Reinitialize,
    .DRV_MIIM_Deinitialize = DRV_MIIM_Deinitialize,
    .DRV_MIIM_Status = DRV_MIIM_Status,
    .DRV_MIIM_Tasks = DRV_MIIM_Tasks,
    .DRV_MIIM_Open = DRV_MIIM_Open,
    // client specific functions
    .DRV_MIIM_Setup = DRV_MIIM_Setup,
    .DRV_MIIM_Close = DRV_MIIM_Close,
    .DRV_MIIM_ClientStatus = DRV_MIIM_ClientStatus,
    .DRV_MIIM_RegisterCallback = DRV_MIIM_RegisterCallback,
    .DRV_MIIM_DeregisterCallback = DRV_MIIM_DeregisterCallback,
    // client operation functions
    .DRV_MIIM_Read = DRV_MIIM_Read,
    .DRV_MIIM_Write = DRV_MIIM_Write,
    .DRV_MIIM_Scan = DRV_MIIM_Scan,
    .DRV_MIIM_OperationResult = DRV_MIIM_OperationResult,
    .DRV_MIIM_OperationAbort = DRV_MIIM_OperationAbort,
    .DRV_MIIM_WriteExt = DRV_MIIM_WriteExt,
    .DRV_MIIM_ReadExt = DRV_MIIM_ReadExt,
};


SYS_MODULE_OBJ DRV_MIIM_Initialize(const SYS_MODULE_INDEX iModule, const SYS_MODULE_INIT* const init )
{
    int ix;
    DRV_MIIM_OBJ* pMiimObj;
    DRV_MIIM_INIT* miimInit = 0;

    _MIIMDebugCond(true, __func__, __LINE__);   // hush compiler warning

    if(iModule >= DRV_MIIM_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    pMiimObj = _DRV_MIIM_GetObject((SYS_MODULE_OBJ)(gDrvMIIMObj + iModule));
    if(pMiimObj != 0)
    {   // already in use
        return (SYS_MODULE_OBJ)pMiimObj ;
    }

    pMiimObj = gDrvMIIMObj + iModule;
    memset(pMiimObj, 0, sizeof(*pMiimObj));

    /* Assign to the local pointer the init data passed */
    if((miimInit = (DRV_MIIM_INIT*) init) == 0)
    {   // cannot do with no init data
        return SYS_MODULE_OBJ_INVALID;
    }

    // create synchronization object
    if(OSAL_SEM_Create(&pMiimObj->objSem, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE)
    {   // failed
        return SYS_MODULE_OBJ_INVALID;
    }

    pMiimObj->objFlags = DRV_MIIM_OBJ_FLAG_IN_USE;      // Set object to be in use
    pMiimObj->objStatus = SYS_STATUS_READY; // Set module state
    pMiimObj->iModule  = iModule;  // Store driver instance
    pMiimObj->miimId = miimInit->miimId; // Store PLIB ID

    // initialize the operation lists
    DRV_MIIM_OP_DCPT* pOpDcpt = pMiimObj->opPool;
    for(ix = 0; ix < sizeof(pMiimObj->opPool) / sizeof(*pMiimObj->opPool); ix++, pOpDcpt++)
    {
        pOpDcpt->qType = DRV_MIIM_QTYPE_FREE;
        Helper_SingleListTailAdd(&pMiimObj->freeOpList, (SGL_LIST_NODE*)pOpDcpt);
    }

#if (DRV_MIIM_CLIENT_OP_PROTECTION)
    DRV_MIIM_CLI_OP_STAMP* pCliStamp = pMiimObj->stampPool;
    for(ix = 0; ix < sizeof(pMiimObj->stampPool) / sizeof(*pMiimObj->stampPool); ix++, pCliStamp++)
    {
        Helper_SingleListTailAdd(&pMiimObj->freeStampList, (SGL_LIST_NODE*)pCliStamp);
    }

#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)


    /* Return the driver handle */
    return (SYS_MODULE_OBJ)pMiimObj ;

}


void DRV_MIIM_Reinitialize(SYS_MODULE_OBJ object, const SYS_MODULE_INIT* const init)
{
    // Currently NOT implemented
}

void DRV_MIIM_Deinitialize( SYS_MODULE_OBJ hSysObj )
{
    int ix;
    DRV_MIIM_CLIENT_DCPT* pClient;

    // avoid another open or client operation now
    DRV_MIIM_OBJ* pMiimObj = _DRV_MIIM_GetObjectAndLock(hSysObj);

    if(pMiimObj != 0)
    {
        // purge all clients
        pClient = pMiimObj->objClients;
        for(ix = 0; ix < sizeof(pMiimObj->objClients) / sizeof(*pMiimObj->objClients); ix++, pClient++)
        {
            if(pClient->clientInUse != 0)
            {   // in use client
                _DRV_MIIM_ClientDeallocate(pClient);
            }
        }

        OSAL_SEM_Delete(&pMiimObj->objSem);
        
        memset(pMiimObj, 0, sizeof(*pMiimObj));
        /* Set the Device Status */
        pMiimObj->objStatus  = SYS_STATUS_UNINITIALIZED;
    }
} 


SYS_STATUS DRV_MIIM_Status( SYS_MODULE_OBJ hSysObj )
{
    DRV_MIIM_OBJ* pMiimObj = _DRV_MIIM_GetObject(hSysObj);

    /* Check for the valid driver object passed */
    if(pMiimObj != 0)
    {
        /* Return the status associated with the driver handle */
        return( pMiimObj->objStatus ) ;
    }

    return SYS_STATUS_ERROR;
} 




// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************

DRV_HANDLE  DRV_MIIM_Open (const SYS_MODULE_INDEX iModule, const DRV_IO_INTENT ioIntent)
{
    DRV_MIIM_CLIENT_DCPT * pClient;
    int  clientIx;
    DRV_HANDLE drvHandle = DRV_HANDLE_INVALID;
    DRV_MIIM_OBJ * pMiimObj = 0;

    while(true)
    {
        if(iModule >= DRV_MIIM_INSTANCES_NUMBER)
        {
            break;
        }

        pMiimObj = _DRV_MIIM_GetObjectAndLock((SYS_MODULE_OBJ)(gDrvMIIMObj + iModule));
        if(pMiimObj == 0)
        {
            break;
        }

        if((ioIntent & DRV_IO_INTENT_EXCLUSIVE) != 0)
        {
            if(pMiimObj->numClients > 0)
            {   // already existing clients
                break;
            }
        }

        /* Allocate the client object and set the flag as in use */
        pClient = _DRV_MIIM_ClientAllocate(pMiimObj, &clientIx) ;
        if(pClient)
        {
            memset(pClient, 0, sizeof(*pClient));
            pClient->clientInUse = true;
            pClient->clientIx = clientIx;
            pClient->parentObj  = pMiimObj;
            pClient->cliStatus = DRV_MIIM_CLIENT_STATUS_READY;

            if((ioIntent & DRV_IO_INTENT_EXCLUSIVE) != 0)
            {
                pMiimObj->objFlags |= DRV_MIIM_OBJ_FLAG_EXCLUSIVE;
            }

            pMiimObj->numClients++;
            drvHandle =  (DRV_HANDLE)pClient;
        }

        break;
    }

    if(pMiimObj != 0)
    {
        _DRV_MIIM_ObjUnlock(pMiimObj);
    }

    return drvHandle;
}

void DRV_MIIM_Close( DRV_HANDLE handle )
{
    DRV_MIIM_CLIENT_DCPT * pClient = _DRV_MIIM_GetClientAndLock(handle, true);

    if(pClient != 0)
    {
        _DRV_MIIM_ClientDeallocate(pClient);
        _DRV_MIIM_ObjUnlock(pClient->parentObj);
    }
}

DRV_MIIM_RESULT DRV_MIIM_Setup(DRV_HANDLE  handle, const DRV_MIIM_SETUP* pSetUp)
{
    DRV_MIIM_CLIENT_DCPT* pClient;
    DRV_MIIM_OBJ* pMiimObj;
    
    DRV_MIIM_RESULT res;

    // basic sanity check
    if(pSetUp == 0)
    {
        return DRV_MIIM_RES_PARAMETER_ERR;
    }

    if((pClient = _DRV_MIIM_GetClientAndLock(handle, true)) == 0)
    {
        return DRV_MIIM_RES_HANDLE_ERR;
    }

    pMiimObj = pClient->parentObj;
    
    // setup the device
    res = DRV_MIIM_DeviceSetup(pMiimObj->miimId, pSetUp);
    
    pMiimObj->objFlags |= DRV_MIIM_OBJ_FLAG_SETUP_DONE;

    _DRV_MIIM_ObjUnlock(pMiimObj);
    return res;
}

void DRV_MIIM_Tasks( SYS_MODULE_OBJ hSysObj )
{
    DRV_MIIM_OP_DCPT *pOpDcpt, *pHead;
    DRV_MIIM_REPORT_ACT repAct;
    DRV_MIIM_OBJ * pMiimObj = _DRV_MIIM_GetObjectAndLock(hSysObj);

    if(pMiimObj == 0)
    {   // minimal sanity check
        return;
    }

    // process the scheduled operations    
    while((pOpDcpt = (DRV_MIIM_OP_DCPT*)pMiimObj->busyOpList.head) != 0)
    {
        _DRV_MIIM_ProcessOp(pMiimObj, pOpDcpt);
        if(pOpDcpt->txferStat >= DRV_MIIM_TXFER_REPORT_STATE)
        {   // somehow complete
            repAct = _DRV_MIIM_ReportOp(pMiimObj, pOpDcpt);
            if(repAct == DRV_MIIM_REPORT_ACT_DELETE)
            {   // we're done
                _DRV_MIIM_ReleaseOpDcpt(pMiimObj, pOpDcpt, &pMiimObj->busyOpList, DRV_MIIM_QTYPE_BUSY);
                continue;
            }
            else if(repAct == DRV_MIIM_REPORT_ACT_WAIT_COMPLETE)
            {   // keep it around for a while
                // remove from queue
                pHead = _DRV_MIIM_OpListRemove(&pMiimObj->busyOpList, DRV_MIIM_QTYPE_BUSY);
                _MIIMAssertCond(pOpDcpt == pHead, __func__, __LINE__);
                _DRV_MIIM_OpListAdd(&pMiimObj->completeOpList, pHead, DRV_MIIM_QTYPE_COMPLETE);
                continue;
            }
        }

        // still ongoing; one operation at a time, no parallel operations could be in progress!
        break;
    }

    _DRV_MIIM_ObjUnlock(pMiimObj);
} 

DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Read(DRV_HANDLE handle, uint16_t rIx, uint16_t phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult)
{
    return _DRV_MIIM_ScheduleOp(handle, rIx, phyAdd, 0, opFlags, pOpResult, DRV_MIIM_OP_READ);
}

DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Write(DRV_HANDLE handle, uint16_t rIx, uint16_t phyAdd, uint16_t wData, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult)
{
    return _DRV_MIIM_ScheduleOp(handle, rIx, phyAdd, wData, opFlags, pOpResult, DRV_MIIM_OP_WRITE);
}

DRV_MIIM_OPERATION_HANDLE DRV_MIIM_Scan(DRV_HANDLE handle, uint16_t rIx, uint16_t phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult)
{
    return _DRV_MIIM_ScheduleOp(handle, rIx, phyAdd, 0, opFlags, pOpResult, DRV_MIIM_OP_SCAN);
}

DRV_MIIM_OPERATION_HANDLE DRV_MIIM_ReadExt(DRV_HANDLE handle, uint16_t rIx, uint16_t phyAdd, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult)
{
    return _DRV_MIIM_ScheduleOp(handle, rIx, phyAdd, 0, opFlags, pOpResult, DRV_MIIM_OP_READ_EXT);
}

DRV_MIIM_OPERATION_HANDLE DRV_MIIM_WriteExt(DRV_HANDLE handle, uint16_t rIx, uint16_t phyAdd, uint32_t wData, DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult)
{
    return _DRV_MIIM_ScheduleOp(handle, rIx, phyAdd, wData, opFlags, pOpResult, DRV_MIIM_OP_WRITE_EXT);
}

DRV_MIIM_RESULT DRV_MIIM_OperationResult(DRV_HANDLE handle, DRV_MIIM_OPERATION_HANDLE opHandle, uint32_t* pOpData)
{
    DRV_MIIM_CLIENT_DCPT* pClient;
    DRV_MIIM_OP_DCPT* pOpDcpt;
    DRV_MIIM_RESULT miimRes;
    DRV_MIIM_OBJ*   pMiimObj = 0;

    while(true)
    {
        if((pClient = _DRV_MIIM_GetClientAndLock(handle, true)) == 0)
        {
            miimRes = DRV_MIIM_RES_HANDLE_ERR;
            break;
        }

        pMiimObj = pClient->parentObj;

        if((pOpDcpt = _DRV_MIIM_GetOpDcpt(pClient, opHandle)) == 0)
        {
            miimRes = DRV_MIIM_RES_OP_HANDLE_ERR;
            break;
        }


        // now this is either an ongoing operation or a completed one with no signal handler
        if(pOpData != 0)
        {   // when scanning, we update the user data even if stale
            if(DRV_MIIM_TXFER_SCAN_STALE <= pOpDcpt->txferStat &&  pOpDcpt->txferStat <= DRV_MIIM_TXFER_DONE)
            {
                *pOpData = pOpDcpt->opData;
            }
        }

        miimRes = _DRV_MIIM_OpResult(pOpDcpt, true);

        if(pOpDcpt->txferStat >= DRV_MIIM_TXFER_DONE)
        {   // this should be in the complete list
            _MIIM_Debug_Oper(pOpDcpt, "release");
            _DRV_MIIM_ReleaseOpDcpt(pMiimObj, pOpDcpt,  &pClient->parentObj->completeOpList, DRV_MIIM_QTYPE_COMPLETE);
        }

        break;
    }

    if(pMiimObj != 0)
    {
        _DRV_MIIM_ObjUnlock(pMiimObj);
    }

    return miimRes;
}


DRV_MIIM_RESULT DRV_MIIM_OperationAbort(DRV_HANDLE handle, DRV_MIIM_OPERATION_HANDLE opHandle)
{
    DRV_MIIM_CLIENT_DCPT* pClient;
    DRV_MIIM_OP_DCPT* pOpDcpt;
    SINGLE_LIST* pList;
    DRV_MIIM_QUEUE_TYPE qType;
    DRV_MIIM_RESULT miimRes;
    DRV_MIIM_OBJ*   pMiimObj = 0;

    while(true)
    {

        if((pClient = _DRV_MIIM_GetClientAndLock(handle, true)) == 0)
        {
            miimRes = DRV_MIIM_RES_HANDLE_ERR;
            break;
        }

        pMiimObj = pClient->parentObj;

        if((pOpDcpt = _DRV_MIIM_GetOpDcpt(pClient, opHandle)) == 0)
        {
            miimRes = DRV_MIIM_RES_OP_HANDLE_ERR;
            break;
        }

        // now this is either an ongoing operation or a completed one with no signal handler
        if(pOpDcpt->txferStat >= DRV_MIIM_TXFER_DONE)
        {
            pList = &pMiimObj->completeOpList;
            qType = DRV_MIIM_QTYPE_COMPLETE;
        }
        else
        {
            pList = &pMiimObj->busyOpList;
            qType = DRV_MIIM_QTYPE_BUSY;
        }

        _DRV_MIIM_ReleaseOpDcpt(pMiimObj, pOpDcpt, pList, qType);
        miimRes = DRV_MIIM_RES_OK;

        break;
    }

    if(pMiimObj != 0)
    {
        _DRV_MIIM_ObjUnlock(pMiimObj);
    }

    return miimRes;
}

DRV_MIIM_CLIENT_STATUS DRV_MIIM_ClientStatus( DRV_HANDLE handle )
{
    DRV_MIIM_CLIENT_DCPT * pClient = _DRV_MIIM_GetClientAndLock(handle, false);

    if(pClient == 0)
    {
        return DRV_MIIM_CLIENT_STATUS_ERROR;
    }

    return pClient->cliStatus; 
}


DRV_MIIM_CALLBACK_HANDLE DRV_MIIM_RegisterCallback(DRV_HANDLE handle, DRV_MIIM_OPERATION_CALLBACK cbFunction, DRV_MIIM_RESULT* pRegResult )
{
    DRV_MIIM_RESULT miimRes;
    DRV_MIIM_CALLBACK_HANDLE cbHandle = 0;
    DRV_MIIM_CLIENT_DCPT* pClient = 0;

    while(true)
    {
        if(cbFunction == 0)
        {
            miimRes = DRV_MIIM_RES_CALLBACK_HANDLE_ERR; 
            break;
        }

        if((pClient = _DRV_MIIM_GetClientAndLock(handle, true)) == 0)
        {
            miimRes = DRV_MIIM_RES_HANDLE_ERR;
            break;
        }

        pClient->cbackHandler = cbFunction;
        miimRes = DRV_MIIM_RES_OK;
        cbHandle = (DRV_MIIM_CALLBACK_HANDLE)cbFunction;

        break;
    }

    if(pRegResult)
    {
        *pRegResult = miimRes;
    }

    if(pClient != 0)
    {
        _DRV_MIIM_ObjUnlock(pClient->parentObj);
    }

    return cbHandle;
}

DRV_MIIM_RESULT DRV_MIIM_DeregisterCallback(DRV_HANDLE handle, DRV_MIIM_CALLBACK_HANDLE cbHandle)
{
    DRV_MIIM_CLIENT_DCPT* pClient;
    DRV_MIIM_RESULT res;

    while(true)
    {

        if((pClient = _DRV_MIIM_GetClientAndLock(handle, true)) == 0)
        {
            res = DRV_MIIM_RES_HANDLE_ERR;
            break;
        }

        if(pClient->cbackHandler != (DRV_MIIM_OPERATION_CALLBACK)cbHandle)
        {
            res = DRV_MIIM_RES_CALLBACK_HANDLE_ERR;
            break;
        }

        pClient->cbackHandler = 0;
        res = DRV_MIIM_RES_OK;

        break;
    }

    if(pClient != 0)
    {
        _DRV_MIIM_ObjUnlock(pClient->parentObj);
    }

    return res;
}


// Helpers

// checks and schedules an operation
// returns DRV_MIIM_RES_OK if success
static DRV_MIIM_OPERATION_HANDLE _DRV_MIIM_ScheduleOp(DRV_HANDLE handle, uint16_t regIx, uint16_t phyAdd, uint32_t opData,
                                                   DRV_MIIM_OPERATION_FLAGS opFlags, DRV_MIIM_RESULT* pOpResult, DRV_MIIM_OP_TYPE opType)
{
    DRV_MIIM_RESULT res;
    DRV_MIIM_OPERATION_HANDLE opHandle = 0;
    DRV_MIIM_CLIENT_DCPT* pClient;
    DRV_MIIM_OBJ* pMiimObj = 0;

    while(true)
    {
        // sanity check
        if(opType != DRV_MIIM_OP_READ_EXT && opType != DRV_MIIM_OP_WRITE_EXT)
        {
           if(regIx > DRV_MIIM_MAX_REG_INDEX_VALUE)
           {
               res = DRV_MIIM_RES_REGISTER_ERR;
               break;
           }

           if(phyAdd > DRV_MIIM_MAX_ADDRESS_VALUE)
           {
               res = DRV_MIIM_RES_ADDRESS_ERR;
               break;
           }
        }

        // get client for this operation
        if((pClient = _DRV_MIIM_GetClientAndLock(handle, true)) == 0)
        {
            res = DRV_MIIM_RES_HANDLE_ERR;
            break;
        }

        // get MIIM object
        pMiimObj = pClient->parentObj;

        if((pMiimObj->objFlags & DRV_MIIM_OBJ_FLAG_SETUP_DONE) == 0)
        {
            res = DRV_MIIM_RES_SETUP_ERR;
            break;
        }

        if((pMiimObj->objFlags & DRV_MIIM_OBJ_FLAG_IS_SCANNING) != 0)
        {
            res = DRV_MIIM_RES_OP_SCAN_ERR;
            break;
        }
        
        // get an operation descriptor
        DRV_MIIM_OP_DCPT* pOpDcpt = _DRV_MIIM_OpListRemove(&pMiimObj->freeOpList, DRV_MIIM_QTYPE_FREE);

        if(pOpDcpt == 0)
        {
            res = DRV_MIIM_RES_BUSY;
            break;
        }

#if (DRV_MIIM_CLIENT_OP_PROTECTION)
        // get client operation stamp
        DRV_MIIM_CLI_OP_STAMP* pCliStamp = (DRV_MIIM_CLI_OP_STAMP*)Helper_SingleListHeadRemove(&pMiimObj->freeStampList);

        if(pCliStamp == 0)
        {   // should not happen - stamps should be in sync with operations!; release
            _MIIMAssertCond(pCliStamp != 0, __func__, __LINE__);
            Helper_SingleListTailAdd(&pMiimObj->freeOpList, (SGL_LIST_NODE*)pOpDcpt);
            res = DRV_MIIM_RES_BUSY;
            break;
        }
#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)

        // all good
        // populate the operation to schedule
        pOpDcpt->opType = opType;
        pOpDcpt->regIx = regIx;
        pOpDcpt->phyAdd = phyAdd;
        pOpDcpt->opFlags = opFlags;
        pOpDcpt->opData = opData;
        pOpDcpt->txferStat = DRV_MIIM_TXFER_START;
        pOpDcpt->pOwner = pClient;

#if (DRV_MIIM_CLIENT_OP_PROTECTION)
        pCliStamp->pOpDcpt = pOpDcpt;
        pCliStamp->pClientDcpt = pClient;
        pCliStamp->tStamp = SYS_TMR_TickCountGet();

        pOpDcpt->pCliStamp = pCliStamp;
        pOpDcpt->tStamp = pCliStamp->tStamp;

        Helper_SingleListTailAdd(&pMiimObj->busyStampList, (SGL_LIST_NODE*)pCliStamp);
        opHandle = pCliStamp;
#else
        opHandle = pOpDcpt;
#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)

        // schedule operation
        if(pOpDcpt->opType == DRV_MIIM_OP_SCAN)
        {   // only one op could be in progress; scan is a continuous one
            pMiimObj->objFlags |= DRV_MIIM_OBJ_FLAG_IS_SCANNING;
        }
        _DRV_MIIM_OpListAdd(&pMiimObj->busyOpList, pOpDcpt, DRV_MIIM_QTYPE_BUSY);

        res = DRV_MIIM_RES_OK;
        break;
    }

    if(pMiimObj != 0)
    {
        _DRV_MIIM_ObjUnlock(pMiimObj);
    }
    
    if(pOpResult)
    {
        *pOpResult = res;
    }
    return opHandle;
}

// Process SMI bus operations. One at a time!
static void _DRV_MIIM_ProcessOp( DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt)
{

    _MIIMAssertCond(pOpDcpt->pOwner->parentObj == pMiimObj, __func__, __LINE__);
    _MIIMAssertCond(pOpDcpt->qType == DRV_MIIM_QTYPE_BUSY, __func__, __LINE__);

    if(pOpDcpt->opType > DRV_MIIM_OP_NONE && pOpDcpt->opType < sizeof(miim_process_op_tbl) / sizeof(*miim_process_op_tbl) + 1)
    {   // process operation
        if(pOpDcpt->txferStat == DRV_MIIM_TXFER_START)
        {
            DRV_MIIM_PortEnable(pMiimObj->miimId);
        }
        miim_process_op_tbl[pOpDcpt->opType - 1](pMiimObj, pOpDcpt);
    }
    else
    {
        _MIIMAssertCond(false, __func__, __LINE__);
        pOpDcpt->txferStat = DRV_MIIM_TXFER_ERROR;
    }

    if(pOpDcpt->txferStat >= DRV_MIIM_TXFER_DONE)
    {
        DRV_MIIM_PortDisable(pMiimObj->miimId);
    }
    _MIIM_Debug_Oper(pOpDcpt, "proc");
}

// operation processing functions
static void MIIM_Process_Read(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt)
{
    switch(pOpDcpt->txferStat)
    {
        case DRV_MIIM_TXFER_START:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // some previous operation; wait
                break;
            }

            // start read
            _MIIM_Debug_Oper(pOpDcpt, "start");
            _MIIM_Debug_Read(pOpDcpt);
            DRV_MIIM_ReadStart(pMiimObj->miimId, pOpDcpt->phyAdd, pOpDcpt->regIx);

            // for read/write operations
            pOpDcpt->txferStat = ((pOpDcpt->opFlags & DRV_MIIM_OPERATION_FLAG_DISCARD) == 0) ? DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE : DRV_MIIM_TXFER_DONE;
            break;

        case DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // wait op to complete
                break;
            }

            pOpDcpt->opData = DRV_MIIM_ReadDataGet(pMiimObj->miimId); // get the read register
            _MIIM_Debug_ReadData(pOpDcpt);
            pOpDcpt->txferStat = DRV_MIIM_TXFER_DONE;
            break;

        default:
            _MIIMAssertCond(false, __func__, __LINE__);
            pOpDcpt->txferStat = DRV_MIIM_TXFER_ERROR;
            break;
    }
}

static void MIIM_Process_Write(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt)
{
    switch(pOpDcpt->txferStat)
    {
        case DRV_MIIM_TXFER_START:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // some previous operation; wait
                break;
            }

            _MIIM_Debug_Write(pOpDcpt);
            DRV_MIIM_WriteData(pMiimObj->miimId, pOpDcpt->phyAdd, pOpDcpt->regIx, pOpDcpt->opData);

            pOpDcpt->txferStat = ((pOpDcpt->opFlags & DRV_MIIM_OPERATION_FLAG_DISCARD) == 0) ? DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE : DRV_MIIM_TXFER_DONE;
            break;

        case DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // wait op to complete
                break;
            }

            pOpDcpt->txferStat = DRV_MIIM_TXFER_DONE;
            break;


        default:
            _MIIMAssertCond(false, __func__, __LINE__);
            pOpDcpt->txferStat = DRV_MIIM_TXFER_ERROR;
            break;
    }
}

static void MIIM_Process_Scan(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt)
{
    switch(pOpDcpt->txferStat)
    {
        case DRV_MIIM_TXFER_START:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // some previous operation; wait
                break;
            }

            // scan
            pOpDcpt->txferStat = DRV_MIIM_ScanEnable(pMiimObj->miimId, pOpDcpt->phyAdd, pOpDcpt->regIx);
            break;

        case DRV_MIIM_TXFER_SCAN_STALE:
        case DRV_MIIM_TXFER_SCAN_VALID:           
            if(DRV_MIIM_GetScanData(pMiimObj->miimId, &pOpDcpt->opData))
            {
                pOpDcpt->txferStat = DRV_MIIM_TXFER_SCAN_VALID;
            }
            break;

        default:
            _MIIMAssertCond(false, __func__, __LINE__);
            pOpDcpt->txferStat = DRV_MIIM_TXFER_ERROR;
            break;
    }
}

static void MIIM_Process_ReadExt(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt)
{
    uint16_t rdData;

    switch(pOpDcpt->txferStat)
    {
        case DRV_MIIM_TXFER_START:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // some previous operation; wait
                break;
            }

            // read 1st part of the extended frame: low 16 bits
            _DRV_MIIM_ReadStartExt(pMiimObj, pOpDcpt, 0);
            pOpDcpt->txferStat = DRV_MIIM_TXFER_EXT_WAIT_PHASE1;
            
            break;

        case DRV_MIIM_TXFER_EXT_WAIT_PHASE1:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // wait op to complete
                break;
            }

            rdData = DRV_MIIM_ReadDataGet(pMiimObj->miimId); // get the read register low 16 bits
            _MIIM_Debug_ExtSMIReadData(pOpDcpt, 0, rdData);
            pOpDcpt->opData = rdData;

            // read 2nd part of the extended frame: high 16 bits
            _DRV_MIIM_ReadStartExt(pMiimObj, pOpDcpt, 1);
            pOpDcpt->txferStat = ((pOpDcpt->opFlags & DRV_MIIM_OPERATION_FLAG_DISCARD) == 0) ? DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE : DRV_MIIM_TXFER_DONE;

            break;

        case DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // wait op to complete
                break;
            }

            rdData = DRV_MIIM_ReadDataGet(pMiimObj->miimId); // get the read register high 16 bits
            _MIIM_Debug_ExtSMIReadData(pOpDcpt, 1, rdData);
            pOpDcpt->opData |= (uint32_t)rdData << 16;

            pOpDcpt->txferStat = DRV_MIIM_TXFER_DONE;
            break;

        default:
            _MIIMAssertCond(false, __func__, __LINE__);
            pOpDcpt->txferStat = DRV_MIIM_TXFER_ERROR;
            break;
    }
}

static void MIIM_Process_WriteExt(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt)
{
    switch(pOpDcpt->txferStat)
    {
        case DRV_MIIM_TXFER_START:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // some previous operation; wait
                break;
            }

            // write 1st part of the extended frame: low 16 bits
            _DRV_MIIM_WriteDataExt(pMiimObj, pOpDcpt, 0);
            pOpDcpt->txferStat = DRV_MIIM_TXFER_EXT_WAIT_PHASE1;
            break;

        case DRV_MIIM_TXFER_EXT_WAIT_PHASE1:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // wait op to complete
                break;
            }

            // write 2nd part of the extended frame: high 16 bits
            _DRV_MIIM_WriteDataExt(pMiimObj, pOpDcpt, 1);

            pOpDcpt->txferStat = ((pOpDcpt->opFlags & DRV_MIIM_OPERATION_FLAG_DISCARD) == 0) ? DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE : DRV_MIIM_TXFER_DONE;
            break;


        case DRV_MIIM_TXFER_RDWR_WAIT_COMPLETE:
            if( DRV_MIIM_PortBusy(pMiimObj->miimId) )
            {   // wait op to complete
                break;
            }

            pOpDcpt->txferStat = DRV_MIIM_TXFER_DONE;
            break;


        default:
            _MIIMAssertCond(false, __func__, __LINE__);
            pOpDcpt->txferStat = DRV_MIIM_TXFER_ERROR;
            break;
    }
}

// seq == 0 means low 16 bits
// seq == 1 means high 16 bits
static void _DRV_MIIM_WriteDataExt(DRV_MIIM_OBJ* pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt, int seq)
{
    // construct the SMI frame
    uint16_t phyAdd = _ExtSMI_Address(pOpDcpt->regIx);
    uint16_t regIx = _ExtSMI_Register(pOpDcpt->regIx, seq);
    uint16_t wData = _ExtSMI_Data(pOpDcpt->opData, seq);
    _MIIM_Debug_ExtSMIWrite(pOpDcpt, phyAdd, regIx, wData, seq);

    DRV_MIIM_WriteData(pMiimObj->miimId, phyAdd, regIx, wData);
}

    
// seq == 0 means low 16 bits
// seq == 1 means high 16 bits
static void _DRV_MIIM_ReadStartExt(DRV_MIIM_OBJ* pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt, int seq)
{
    // construct the SMI frame
    uint16_t phyAdd = _ExtSMI_Address(pOpDcpt->regIx);
    uint16_t regIx = _ExtSMI_Register(pOpDcpt->regIx, seq);
    _MIIM_Debug_ExtSMIRead(pOpDcpt, phyAdd, regIx, seq);

    DRV_MIIM_ReadStart(pMiimObj->miimId, phyAdd, regIx);
}

// reports the result of an operation
// returns the action needded to be taken:
//  - delete, complete, no action
static DRV_MIIM_REPORT_ACT _DRV_MIIM_ReportOp(DRV_MIIM_OBJ * pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt)
{
    DRV_MIIM_REPORT_ACT repAct;
    bool isReported = false;
    DRV_MIIM_CLIENT_DCPT* pClient = pOpDcpt->pOwner;

    _MIIMAssertCond(pClient->parentObj == pMiimObj, __func__, __LINE__);

    while(true)
    {
        if((pOpDcpt->opFlags & DRV_MIIM_OPERATION_FLAG_DISCARD) == 0)
        {   // need to signal to client that we're done...
            if(pClient->cbackHandler != 0)
            {   // notify the client
                DRV_MIIM_RESULT opResult = _DRV_MIIM_OpResult(pOpDcpt, true);
#if (DRV_MIIM_CLIENT_OP_PROTECTION)
                (*pClient->cbackHandler)((DRV_HANDLE)pClient, pOpDcpt->pCliStamp, opResult, pOpDcpt->opData);
#else
                (*pClient->cbackHandler)((DRV_HANDLE)pClient, pOpDcpt, opResult, pOpDcpt->opData);
#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)
                isReported = true;
            }
        }
        else
        {   // discarded ops don't need reporting
            isReported = true;
        }

        if(pOpDcpt->opType == DRV_MIIM_OP_SCAN)
        {   // a scan operation will remain active until aborted
            repAct = DRV_MIIM_REPORT_ACT_NONE;
        }
        else
        {   // either is reported and we're done with it or wait until user asks for it
            repAct = isReported ? DRV_MIIM_REPORT_ACT_DELETE : DRV_MIIM_REPORT_ACT_WAIT_COMPLETE;
        }

        break;
    }

    return repAct;
}

// translate an operation current txferStat to a client DRV_MIIM_RESULT
// object should be locked
static DRV_MIIM_RESULT _DRV_MIIM_OpResult(DRV_MIIM_OP_DCPT* pOpDcpt, bool scanAck)
{
    if(DRV_MIIM_TXFER_START <= pOpDcpt->txferStat &&  pOpDcpt->txferStat < DRV_MIIM_TXFER_SCAN_VALID)
    {
        return DRV_MIIM_RES_PENDING;
    }
    else if(pOpDcpt->txferStat <= DRV_MIIM_TXFER_DONE)
    {
        if(scanAck && pOpDcpt->txferStat == DRV_MIIM_TXFER_SCAN_VALID)
        {
            pOpDcpt->txferStat = DRV_MIIM_TXFER_SCAN_STALE;
        }
        return DRV_MIIM_RES_OK;
    }

    // should not happen
    _MIIMAssertCond(false, __func__, __LINE__);
    return DRV_MIIM_RES_OP_INTERNAL_ERR;
}

// returns a operation descriptor from DRV_MIIM_OPERATION_HANDLE
static DRV_MIIM_OP_DCPT* _DRV_MIIM_GetOpDcpt(DRV_MIIM_CLIENT_DCPT* pClient, DRV_MIIM_OPERATION_HANDLE opHandle)
{
    DRV_MIIM_OP_DCPT* pOpDcpt;

#if (DRV_MIIM_CLIENT_OP_PROTECTION)
    // check that ths client has rights for this operation
    DRV_MIIM_CLI_OP_STAMP* pCliStamp = (DRV_MIIM_CLI_OP_STAMP*)opHandle;

    pOpDcpt = 0;
    if(pCliStamp != 0 && pCliStamp->pClientDcpt == pClient)
    {
        if(pCliStamp->pOpDcpt->pCliStamp == pCliStamp)
        {   // finally check the time stamps
            if(pCliStamp->tStamp == pCliStamp->pOpDcpt->tStamp)
            {
                pOpDcpt = pCliStamp->pOpDcpt;
            }
        }
    }
#else
    pOpDcpt = (DRV_MIIM_OP_DCPT*)opHandle;
#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)

    if(pOpDcpt != 0 && (pOpDcpt->qType == DRV_MIIM_QTYPE_BUSY || pOpDcpt->qType == DRV_MIIM_QTYPE_COMPLETE))
    {
        if(pOpDcpt->txferStat < DRV_MIIM_TXFER_DONE)
        {
            _MIIMAssertCond(pOpDcpt->qType == DRV_MIIM_QTYPE_BUSY, __func__, __LINE__);
        }
        else
        {
            _MIIMAssertCond(pOpDcpt->qType == DRV_MIIM_QTYPE_COMPLETE, __func__, __LINE__);
        }

        return pOpDcpt;
    }

    return 0;
}


static void _DRV_MIIM_ReleaseOpDcpt(DRV_MIIM_OBJ* pMiimObj, DRV_MIIM_OP_DCPT* pOpDcpt, SINGLE_LIST* pRemList, DRV_MIIM_QUEUE_TYPE qType)
{
    DRV_MIIM_OP_DCPT* pRemNode;
#if (DRV_MIIM_CLIENT_OP_PROTECTION)
    DRV_MIIM_CLI_OP_STAMP *pCliStamp, *pRemStamp;
    pCliStamp = pOpDcpt->pCliStamp;
#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)
    bool wasScan = false;

    _MIIMAssertCond(pOpDcpt->qType == qType, __func__, __LINE__);
    pRemNode =  (DRV_MIIM_OP_DCPT*)Helper_SingleListNodeRemove(pRemList, (SGL_LIST_NODE*)pOpDcpt);

    // make sure we could remove it!
    if(pRemNode == pOpDcpt)
    {
        wasScan = pOpDcpt->opType == DRV_MIIM_OP_SCAN; 
        memset(pOpDcpt, 0, sizeof(*pOpDcpt));
        _DRV_MIIM_OpListAdd(&pMiimObj->freeOpList, pOpDcpt, DRV_MIIM_QTYPE_FREE);
    }
    else
    {
        _MIIMAssertCond(false, __func__, __LINE__);
    }
#if (DRV_MIIM_CLIENT_OP_PROTECTION)
    pRemStamp = (DRV_MIIM_CLI_OP_STAMP*)Helper_SingleListNodeRemove(&pMiimObj->busyStampList, (SGL_LIST_NODE*)pCliStamp);
    if(pRemStamp == pCliStamp)
    {
        _MIIMAssertCond(pCliStamp->pOpDcpt == pOpDcpt, __func__, __LINE__);
        memset(pCliStamp, 0, sizeof(*pCliStamp));
        Helper_SingleListTailAdd(&pMiimObj->freeStampList, (SGL_LIST_NODE*)pRemStamp);
    }
    else
    {
        _MIIMAssertCond(false, __func__, __LINE__);
    }
#endif  // (DRV_MIIM_CLIENT_OP_PROTECTION)

    if(wasScan)
    {        
        DRV_MIIM_ScanDisable(pMiimObj->miimId);
        pMiimObj->objFlags &= ~DRV_MIIM_OBJ_FLAG_IS_SCANNING;
    }
    
    if(qType == DRV_MIIM_QTYPE_BUSY)
    {
        DRV_MIIM_PortDisable(pMiimObj->miimId);
    }
}

static DRV_MIIM_CLIENT_DCPT* _DRV_MIIM_ClientAllocate( DRV_MIIM_OBJ* pMiimObj, int* pCliIx )
{
    int ix;

    DRV_MIIM_CLIENT_DCPT* pClient = pMiimObj->objClients;
    for(ix = 0; ix < sizeof(pMiimObj->objClients) / sizeof(*pMiimObj->objClients); ix++, pClient++)
    {
        if(pClient->clientInUse == 0)
        {   // available client
            *pCliIx = ix;
            return pClient;
        }
    }

    return 0;
}


// de-allocates a client
// MIIM object should be locked
static void _DRV_MIIM_ClientDeallocate( DRV_MIIM_CLIENT_DCPT* pClient)
{
    _DRV_MIIM_PurgeClientOp(pClient);
    pClient->clientInUse = false ;
    pClient->cliStatus = DRV_CLIENT_STATUS_ERROR;
    pClient->parentObj->numClients--;
    pClient->parentObj->objFlags &= ~DRV_MIIM_OBJ_FLAG_EXCLUSIVE;
}


// search the busy and complete lists and remove
// all operations owned by this client
static void _DRV_MIIM_PurgeClientOp(DRV_MIIM_CLIENT_DCPT* pClient)
{
    DRV_MIIM_OP_DCPT* pOpDcpt;
    SINGLE_LIST delList, busyList, completeList;
    DRV_MIIM_OBJ* pMiimObj = pClient->parentObj;

    Helper_SingleListInitialize(&delList);
    Helper_SingleListInitialize(&busyList);
    Helper_SingleListInitialize(&completeList);

    // check the ongoing operations
    while((pOpDcpt = _DRV_MIIM_OpListRemove(&pMiimObj->busyOpList, DRV_MIIM_QTYPE_BUSY)) != 0)
    {
        if(pOpDcpt->pOwner == pClient)
        {
            Helper_SingleListTailAdd(&delList, (SGL_LIST_NODE*)pOpDcpt);
        }
        else
        {
            Helper_SingleListTailAdd(&busyList, (SGL_LIST_NODE*)pOpDcpt);
        }
    }

    pMiimObj->busyOpList = busyList;

    while((pOpDcpt = (DRV_MIIM_OP_DCPT*)delList.head) != 0)
    {
        _DRV_MIIM_ReleaseOpDcpt(pMiimObj, pOpDcpt, &delList, DRV_MIIM_QTYPE_BUSY);
    }

    // same for completed operations
    while((pOpDcpt = _DRV_MIIM_OpListRemove(&pMiimObj->completeOpList, DRV_MIIM_QTYPE_COMPLETE)) != 0)
    {
        if(pOpDcpt->pOwner == pClient)
        {
            Helper_SingleListTailAdd(&delList, (SGL_LIST_NODE*)pOpDcpt);
        }
        else
        {
            Helper_SingleListTailAdd(&completeList, (SGL_LIST_NODE*)pOpDcpt);
        }
    }

    pMiimObj->completeOpList = completeList;

    while((pOpDcpt = (DRV_MIIM_OP_DCPT*)delList.head) != 0)
    {
        _DRV_MIIM_ReleaseOpDcpt(pMiimObj, pOpDcpt, &delList, DRV_MIIM_QTYPE_COMPLETE);
    }

}

void  Helper_SingleListInitialize(SINGLE_LIST* pL)
{
    pL->head = pL->tail = 0;
    pL->nNodes = 0;
}

void  Helper_SingleListTailAdd(SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{
    pN->next = 0;
    if(pL->tail == 0)
    {
        pL->head = pL->tail = pN;
    }
    else
    {
        pL->tail->next = pN;
        pL->tail = pN;
    }
    pL->nNodes++;
}


SGL_LIST_NODE*  Helper_SingleListHeadRemove(SINGLE_LIST* pL)
{
    SGL_LIST_NODE* pN = pL->head;
    if(pN)
    {
        if(pL->head == pL->tail)
        {
            pL->head = pL->tail = 0;
        }
        else
        {
            pL->head = pN->next;
        }
        pL->nNodes--;
    }

    return pN;
}

// removes a node somewhere in the middle
// Note: this is lengthy!
// Use a double linked list if faster operation needed!



SGL_LIST_NODE*  Helper_SingleListNodeRemove(SINGLE_LIST* pL, SGL_LIST_NODE* pN)
{
    if(pN == pL->head)
    {
        return Helper_SingleListHeadRemove(pL);
    }
    else
    {
        SGL_LIST_NODE* prev;
        for(prev = pL->head; prev != 0 && prev->next != pN; prev = prev->next);
        if(prev == 0)
        {   // no such node
            return 0;
        }
        // found it
        prev->next = pN->next;
        if(pN == pL->tail)
        {
            pL->tail = prev;
        }
        pL->nNodes--;
    }

    return pN;
}

 

/*******************************************************************************
End of File
*/

