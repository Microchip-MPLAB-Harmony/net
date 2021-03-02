/*******************************************************************************
  Ethernet PHY Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ethphy.c

  Summary:
    Ethernet PHY Device Driver Dynamic Implementation

  Description:
    The Ethernet PHY device driver provides a simple interface to manage the
    Ethernet PHY modules on Microchip microcontrollers.  This file Implements
    the core interface routines for the Ethernet PHY driver.

    While building the driver from source, ALWAYS use this file in the build.
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


// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "driver/ethphy/src/drv_ethphy_local.h"
#include "system/sys_time_h2_adapter.h"
#include "system/debug/sys_debug.h"


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************


// Local Definitions
//
#define PROT_802_3  0x01    // IEEE 802.3 capability
#define MAC_COMM_CPBL_MASK  (_BMSTAT_BASE10T_HDX_MASK|_BMSTAT_BASE10T_FDX_MASK|_BMSTAT_BASE100TX_HDX_MASK|_BMSTAT_BASE100TX_FDX_MASK)
// all comm capabilities our MAC supports

// local prototypes
// debug
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC) != 0)
volatile int _PhyStayAssertLoop = 0;
static void _PhyAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PHY Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_PhyStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _PhyStayCondLoop = 0;
static void _PhyDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PHY Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_PhyStayCondLoop != 0);
    }
}

#else
#define _PhyAssertCond(cond, message, lineNo)
#define _PhyDebugCond(cond, message, lineNo)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC)

#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_PHASE) != 0)
static void _PhyDetectPhase(uint16_t detectPhase)
{
    static uint16_t prevState = 0xffff;
    if(detectPhase != prevState)
    {
        prevState = detectPhase;
        SYS_CONSOLE_PRINT("PHY detect phase: %d\r\n", detectPhase);
    }
}
#else
#define _PhyDetectPhase(detectPhase)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_PHASE)

#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_VALUES) != 0)
static void _PhyDetectValue(uint16_t rVal, bool writeOp)
{
    SYS_CONSOLE_PRINT("PHY detect value: %s 0x%04x\r\n", writeOp ? "write" : "read", rVal);
}
#else
#define _PhyDetectValue(rVal, writeOp)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_VALUES)


typedef void (*_DRV_PHY_OperPhaseF)(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void _DRV_ETHPHY_ClientOpNone(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpSetup(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpLinkStat(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpNegComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpNegResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpNegRestart(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const _DRV_PHY_OperPhaseF _DRV_PHY_ClientOpTbl[] =
{
    _DRV_ETHPHY_ClientOpNone,           // DRV_ETHPHY_CLIENT_OP_TYPE_NONE 
    _DRV_ETHPHY_ClientOpSetup,          // DRV_ETHPHY_CLIENT_OP_TYPE_SETUP
    _DRV_ETHPHY_ClientOpLinkStat,       // DRV_ETHPHY_CLIENT_OP_TYPE_LINK_STAT
    _DRV_ETHPHY_ClientOpNegComplete,    // DRV_ETHPHY_CLIENT_OP_TYPE_NEG_COMPLETE
    _DRV_ETHPHY_ClientOpNegResult,      // DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESULT
    _DRV_ETHPHY_ClientOpNegRestart,     // DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESTART
    _DRV_ETHPHY_ClientOpReset,          // DRV_ETHPHY_CLIENT_OP_TYPE_RESET
};


static void _DRV_ETHPHY_SetupPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseDetect(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate(DRV_ETHPHY_CLIENT_OBJ * hClientObj);



static const _DRV_PHY_OperPhaseF _DRV_PHY_SetupPhasesTbl[] = 
{
    _DRV_ETHPHY_SetupPhaseIdle,
    _DRV_ETHPHY_SetupPhaseDetect,
    _DRV_ETHPHY_SetupPhaseReset,
    _DRV_ETHPHY_SetupPhaseNegotiate,
};


static void _DRV_ETHPHY_LinkStatPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_LinkStatPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_LinkStatPhaseRefresh(DRV_ETHPHY_CLIENT_OBJ * hClientObj);



static const _DRV_PHY_OperPhaseF _DRV_PHY_LinkStatPhasesTbl[] = 
{
    _DRV_ETHPHY_LinkStatPhaseIdle,
    _DRV_ETHPHY_LinkStatPhaseRead,
    _DRV_ETHPHY_LinkStatPhaseRefresh,
};


static void _DRV_ETHPHY_NegCompletePhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseRead1(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseRead2(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseResultNoWait(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseAN_Restart(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseAN_Complete(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void _DRV_PHY_NegCompleteSetOperResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t phyBmConReg, uint16_t phyStatReg);

static const _DRV_PHY_OperPhaseF _DRV_PHY_NegCompletePhasesTbl[] = 
{
    _DRV_ETHPHY_NegCompletePhaseIdle,
    _DRV_ETHPHY_NegCompletePhaseRead1,
    _DRV_ETHPHY_NegCompletePhaseRead2,
    _DRV_ETHPHY_NegCompletePhaseResultNoWait,
    _DRV_ETHPHY_NegCompletePhaseAN_Restart,
    _DRV_ETHPHY_NegCompletePhaseAN_Complete,
};


static void _DRV_ETHPHY_NegResultPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_BMSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANEXP(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANLPAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void _DRV_PHY_NegResultSetOperResult(DRV_ETHPHY_CLIENT_OBJ* hClientObj, DRV_ETHPHY_LINK_STATUS linkStatus,
                                    TCPIP_ETH_OPEN_FLAGS openFlags, TCPIP_ETH_PAUSE_TYPE pauseType, DRV_ETHPHY_RESULT res);

static const _DRV_PHY_OperPhaseF _DRV_PHY_NegResultPhasesTbl[] = 
{
    _DRV_ETHPHY_NegResultPhaseIdle,
    _DRV_ETHPHY_NegResultPhase_BMSTAT,
    _DRV_ETHPHY_NegResultPhase_ANEXP,
    _DRV_ETHPHY_NegResultPhase_ANLPAD,
    _DRV_ETHPHY_NegResultPhase_ANAD,
};

static void _DRV_ETHPHY_NegRestartPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegRestartPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegRestartPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const _DRV_PHY_OperPhaseF _DRV_PHY_NegRestartPhasesTbl[] = 
{
    _DRV_ETHPHY_NegRestartPhaseIdle,
    _DRV_ETHPHY_NegRestartPhaseRead,
    _DRV_ETHPHY_NegRestartPhaseWrite,
};

static void _DRV_ETHPHY_ResetPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ResetPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ResetPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const _DRV_PHY_OperPhaseF _DRV_PHY_ResetPhasesTbl[] = 
{
    _DRV_ETHPHY_ResetPhaseIdle,
    _DRV_ETHPHY_ResetPhaseWrite,
    _DRV_ETHPHY_ResetPhaseRead,
};

// local functions
//

static void _DRV_PHY_SetOperDoneResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_RESULT res);

static DRV_ETHPHY_RESULT _DRV_ETHPHY_DefaultDetect( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE hClientObj);

static DRV_ETHPHY_LINK_STATUS _Phy2LinkStat(__BMSTATbits_t phyStat)
{
    DRV_ETHPHY_LINK_STATUS  linkStat;

    linkStat = (phyStat.LINK_STAT)?DRV_ETHPHY_LINK_ST_UP:DRV_ETHPHY_LINK_ST_DOWN;
    if(phyStat.REM_FAULT)
    {
        linkStat|=DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
    }

    return linkStat;

}

// starts a read/write transfer
// returns:
//      - < 0 an error occurred
//      - DRV_ETHPHY_SMI_TXFER_RES_BUSY: transaction needs to be retried
//      - DRV_ETHPHY_SMI_TXFER_RES_SCHEDULED: transaction started (read)
//      - DRV_ETHPHY_SMI_TXFER_RES_DONE: transaction done (simple write)
static DRV_ETHPHY_SMI_TXFER_RES _DRV_PHY_SMITransferStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_MIIM_OPERATION_HANDLE miimOpHandle;
    DRV_MIIM_RESULT miimRes;
    DRV_ETHPHY_SMI_TXFER_OP_STATUS newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
    DRV_ETHPHY_SMI_TXFER_RES txferRes;
    DRV_MIIM_OPERATION_FLAGS opFlags;
    DRV_ETHPHY_SMI_XFER_TYPE txType = hClientObj->smiTxferType;

    if(hClientObj->miimOpHandle != 0)
    {   // no operations queued, need to wait the end of the previous one
        return DRV_ETHPHY_SMI_TXFER_RES_BUSY;
    }

    if(txType == DRV_ETHPHY_SMI_XFER_TYPE_READ)
    {   // start read
        miimOpHandle = hClientObj->pMiimBase->DRV_MIIM_Read(hClientObj->miimHandle, hClientObj->smiRIx, hClientObj->smiPhyAddress, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
        newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE;
    }
    else if(txType == DRV_ETHPHY_SMI_XFER_TYPE_WRITE || txType == DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE)
    {   // start write
        if(txType == DRV_ETHPHY_SMI_XFER_TYPE_WRITE)
        {
            opFlags = DRV_MIIM_OPERATION_FLAG_DISCARD;
            newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
        }
        else
        {
            opFlags = DRV_MIIM_OPERATION_FLAG_NONE;
            newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE;
        }

        miimOpHandle = hClientObj->pMiimBase->DRV_MIIM_Write(hClientObj->miimHandle, hClientObj->smiRIx, hClientObj->smiPhyAddress, hClientObj->smiData, opFlags, &miimRes);
    }
    else
    {   // no other/scan operations allowed that take complete control of DRV_MIIM!
        _PhyAssertCond(false, __func__, __LINE__);
        return DRV_ETHPHY_SMI_TXFER_RES_ILLEGAL;
    }

    if(miimOpHandle == 0)
    {   // MIIM operation failed or needs to be retried
        if(miimRes == DRV_MIIM_RES_BUSY)
        {
            return DRV_ETHPHY_SMI_TXFER_RES_BUSY;   // retry
        }
        else 
        {   // some real error
            _PhyDebugCond(false, __func__, __LINE__);
            return DRV_ETHPHY_SMI_TXFER_RES_MIIM_ERROR;
        }
    }

    if(newTxferStatus == DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE) 
    {
        hClientObj->miimOpHandle = miimOpHandle;
        txferRes = DRV_ETHPHY_SMI_TXFER_RES_SCHEDULED; 
    }
    else
    {
        txferRes = DRV_ETHPHY_SMI_TXFER_RES_DONE; 
    }

    hClientObj->smiTxferStatus = newTxferStatus;
    return txferRes;

}

// waits for a read/write transfer to complete
// returns:
//      - < 0 an error occurred
//      - DRV_ETHPHY_SMI_TXFER_RES_WAIT: not yet complete, call again
//      - DRV_ETHPHY_SMI_TXFER_RES_DONE: transaction completed succesfully
static DRV_ETHPHY_SMI_TXFER_RES _DRV_PHY_SMITransferWaitComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_SMI_TXFER_RES txferRes;
    DRV_MIIM_RESULT miimRes;
    uint16_t  opData;

    if(hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE)
    {   // nothing to wait for; but some functions may call this if a write operation was failed by MIIM and needs retry
        _PhyDebugCond(false, __func__, __LINE__);
        return DRV_ETHPHY_SMI_TXFER_RES_DONE;
    } 

    if(hClientObj->miimHandle == 0)
    {   // shouldn't happen
        _PhyAssertCond(false, __func__, __LINE__);
        return DRV_ETHPHY_SMI_TXFER_RES_ILLEGAL;
    } 

    miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, hClientObj->miimOpHandle, &opData);
    if(miimRes == DRV_MIIM_RES_PENDING)
    {   // wait op to complete
        return DRV_ETHPHY_SMI_TXFER_RES_WAIT;
    }

    if(miimRes < 0)
    {   // some error has occurred
        _PhyDebugCond(false, __func__, __LINE__);
        txferRes = DRV_ETHPHY_SMI_TXFER_RES_MIIM_ERROR; 
    }
    else
    {
        txferRes = DRV_ETHPHY_SMI_TXFER_RES_DONE; 
        if(hClientObj->smiTxferType == DRV_ETHPHY_SMI_XFER_TYPE_READ)
        {
            hClientObj->smiData = opData; // set the read result
        }
    }

    hClientObj->miimOpHandle = 0;  // operation done
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
    return txferRes;
}

// waits for a SMI transfer to complete
// returns true if it is done and can move on
// false if it failed or need to wait
static bool _DRV_PHY_SMITransfer_Wait(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_SMI_TXFER_RES opRes = _DRV_PHY_SMITransferWaitComplete(hClientObj);

    if(opRes < 0)
    {   // error
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
        return false;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_WAIT)
    {   // retry
        return false;
    }

    return true;
}

// starts an SMI read transfer
// returns true if went through
// false if need to wait or there was an error
static bool _DRV_PHY_SMIReadStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_READ;
    hClientObj->smiPhyAddress = hClientObj->hDriver->phyAddress;

    DRV_ETHPHY_SMI_TXFER_RES opRes = _DRV_PHY_SMITransferStart(hClientObj);

    if(opRes < 0)
    {   // error
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
        return false;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {   // retry
        return false;
    }

    // went through
    return true;
}

static DRV_ETHPHY_SMI_TXFER_RES _DRV_PHY_SMIReadStartEx(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, int phyAdd)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_READ;
    hClientObj->smiPhyAddress = phyAdd;

    return _DRV_PHY_SMITransferStart(hClientObj);
}

// starts an SMI write transfer
// returns true if went through
// false if need to retry or there was an error
static bool _DRV_PHY_SMIWriteStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, uint16_t wData)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_WRITE;
    hClientObj->smiData =  wData;
    hClientObj->smiPhyAddress = hClientObj->hDriver->phyAddress;

    DRV_ETHPHY_SMI_TXFER_RES opRes = _DRV_PHY_SMITransferStart(hClientObj);

    if(opRes < 0)
    {   // error
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
        return false;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {   // retry
        return false;
    }

    // went through
    return true;
}

static DRV_ETHPHY_SMI_TXFER_RES _DRV_PHY_SMIWriteStartEx(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, uint16_t wData, int phyAdd)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_WRITE;
    hClientObj->smiData =  wData;
    hClientObj->smiPhyAddress = phyAdd;

   return  _DRV_PHY_SMITransferStart(hClientObj);
}

static void _DRV_PHY_SetOperPhase(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t operPhase, uint16_t operSubPhase)
{
    hClientObj->operPhase = operPhase;
    hClientObj->operSubPhase = operSubPhase;
}

static void _DRV_PHY_SetOperDoneResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_RESULT res)
{
    hClientObj->operPhase =  hClientObj->operSubPhase = 0;
    hClientObj->operType = DRV_ETHPHY_CLIENT_OP_TYPE_NONE;
    hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_READY;
    hClientObj->operRes = res;
    if(res < 0)
    {
        SYS_CONSOLE_PRINT("DRV_PHY operation error: %d\r\n", res);
    }
}

static void _DRV_PHY_SetOperStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE opType, DRV_ETHPHY_RESULT res)
{
    hClientObj->operType = opType;
    hClientObj->operPhase = hClientObj->operSubPhase = 0; 
    hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_BUSY; 
    hClientObj->operRes = res;
}


// *****************************************************************************
/* Driver Hardware instance objects.

  Summary:
    Defines the hardware instances objects that are available on the part

  Description:
    This data type defines the hardware instance objects that are available on
    the part, so as to capture the hardware state of the instance.

  Remarks:
    Not all modes are available on all micro-controllers.
*/

#if DRV_ETHPHY_INSTANCES_NUMBER < DRV_ETHPHY_INDEX_COUNT 
#define _DRV_ETHPHY_INSTANCES_NUMBER    DRV_ETHPHY_INSTANCES_NUMBER
#else
#define _DRV_ETHPHY_INSTANCES_NUMBER    DRV_ETHPHY_INDEX_COUNT
#endif
static DRV_ETHPHY_OBJ              gDrvETHPHYObj[_DRV_ETHPHY_INSTANCES_NUMBER];


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static DRV_ETHPHY_CLIENT_OBJ * _DRV_ETHPHY_ClientObjectAllocate( DRV_ETHPHY_OBJ* hPhyObj, int* pCliIx )
{
    int ix;

    DRV_ETHPHY_CLIENT_OBJ* pClientObj = hPhyObj->objClients;
    for(ix = 0; ix < sizeof(hPhyObj->objClients) / sizeof(*hPhyObj->objClients); ix++, pClientObj++)
    {
        if(pClientObj->clientInUse == 0)
        {   // available client
            *pCliIx = ix;
            return pClientObj;
        }
    }

    return 0;
}



// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// The default basic PHY driver object
//
const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_Default = 
{
     .DRV_ETHPHY_Initialize =             DRV_ETHPHY_Initialize,
     .DRV_ETHPHY_Reinitialize =           DRV_ETHPHY_Reinitialize,
     .DRV_ETHPHY_Deinitialize =           DRV_ETHPHY_Deinitialize,
     .DRV_ETHPHY_Status =                 DRV_ETHPHY_Status,
     .DRV_ETHPHY_Tasks =                  DRV_ETHPHY_Tasks,
     .DRV_ETHPHY_Open =                   DRV_ETHPHY_Open,
     .DRV_ETHPHY_Close =                  DRV_ETHPHY_Close,
     .DRV_ETHPHY_ClientStatus =           DRV_ETHPHY_ClientStatus,
     .DRV_ETHPHY_ClientOperationResult =  DRV_ETHPHY_ClientOperationResult,
     .DRV_ETHPHY_ClientOperationAbort =   DRV_ETHPHY_ClientOperationAbort,
     .DRV_ETHPHY_PhyAddressGet =          DRV_ETHPHY_PhyAddressGet,
     .DRV_ETHPHY_Setup =                  DRV_ETHPHY_Setup,
     .DRV_ETHPHY_RestartNegotiation =     DRV_ETHPHY_RestartNegotiation,
     .DRV_ETHPHY_HWConfigFlagsGet =       DRV_ETHPHY_HWConfigFlagsGet,
     .DRV_ETHPHY_NegotiationIsComplete =  DRV_ETHPHY_NegotiationIsComplete,
     .DRV_ETHPHY_NegotiationResultGet =   DRV_ETHPHY_NegotiationResultGet,
     .DRV_ETHPHY_LinkStatusGet =          DRV_ETHPHY_LinkStatusGet,
     .DRV_ETHPHY_Reset =                  DRV_ETHPHY_Reset,
     .DRV_ETHPHY_VendorDataGet =          DRV_ETHPHY_VendorDataGet,
     .DRV_ETHPHY_VendorDataSet =          DRV_ETHPHY_VendorDataSet,
     .DRV_ETHPHY_VendorSMIReadStart =     DRV_ETHPHY_VendorSMIReadStart,
     .DRV_ETHPHY_VendorSMIReadResultGet = DRV_ETHPHY_VendorSMIReadResultGet,
     .DRV_ETHPHY_VendorSMIWriteStart =    DRV_ETHPHY_VendorSMIWriteStart,
};

// self reference to the base object itself
static const DRV_ETHPHY_OBJECT_BASE* gDrvEthBaseObj = &DRV_ETHPHY_OBJECT_BASE_Default;


//******************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_ETHPHY_Initialize( const SYS_MODULE_INDEX    drvIndex,
                                          const SYS_MODULE_INIT     * const init )

  Summary:
    Initializes hardware and data for the given instance of the ETHPHY module

  Description:
    This routine initializes hardware for the instance of the ETHPHY module,
    using the hardware initialization given data.  It also initializes all
    necessary internal data.

  Parameters:
    drvIndex        - Identifies the driver instance to be initialized
    init            - Pointer to the data structure containing all data
                      necessary to initialize the hardware. This pointer may
                      be null if no data is required and static initialization
                      values are to be used.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, it returns SYS_MODULE_OBJ_INVALID.
*/

SYS_MODULE_OBJ DRV_ETHPHY_Initialize( const SYS_MODULE_INDEX  iModule,
                                      const SYS_MODULE_INIT   * const init )
{
    DRV_ETHPHY_OBJ * hSysObj;
    DRV_ETHPHY_INIT *ethphyInit = NULL;

    if ( iModule >= _DRV_ETHPHY_INSTANCES_NUMBER || gDrvETHPHYObj[iModule].objInUse == true )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    hSysObj = gDrvETHPHYObj + iModule;

    /* Assign to the local pointer the init data passed */
    ethphyInit = ( DRV_ETHPHY_INIT * ) init;

    if((hSysObj->pMiimBase = ethphyInit->pMiimObject) == 0)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    hSysObj->miimIndex = ethphyInit->miimIndex;

    hSysObj->objInUse = true;      // Set object to be in use
    hSysObj->busInUse = 0;
    hSysObj->numClients = 0;
    hSysObj->status = SYS_STATUS_READY; // Set module state
    hSysObj->iModule  = iModule;  // Store driver instance
    hSysObj->ethphyId = ethphyInit->ethphyId; // Store PLIB ID

    // Assign External PHY Service Functions
    hSysObj->pPhyObj = ethphyInit->pPhyObject;

    memset(hSysObj->objClients, 0, sizeof(hSysObj->objClients));

    // hush compiler warning
    _PhyAssertCond(true, __func__, __LINE__);
    _PhyDebugCond(true, __func__, __LINE__);

    /* Return the driver handle */
    return ( SYS_MODULE_OBJ )hSysObj ;

}


void DRV_ETHPHY_Reinitialize( SYS_MODULE_OBJ        object ,
                              const SYS_MODULE_INIT * const init )
{
    DRV_ETHPHY_OBJ * hSysObj = (DRV_ETHPHY_OBJ *) object ;
    DRV_ETHPHY_INIT * ethphyInit = (DRV_ETHPHY_INIT *)init;

    /* Check for the valid driver object passed */
    if( NULL != hSysObj)
    {

        hSysObj->numClients = 0;
        hSysObj->status = SYS_STATUS_READY; // Set module state
        hSysObj->ethphyId = ethphyInit->ethphyId; // Store PLIB ID

        // Assign External PHY Service Functions
        hSysObj->pPhyObj = ethphyInit->pPhyObject;

        memset(hSysObj->objClients, 0, sizeof(hSysObj->objClients));
    }
}



void DRV_ETHPHY_Deinitialize( SYS_MODULE_OBJ hSysObj )
{
    DRV_ETHPHY_OBJ * hObj = (DRV_ETHPHY_OBJ *) hSysObj ;

    /* Check for the valid driver object passed */
    if( NULL != hObj)
    {

        /* Set the Device Status */
        hObj->status  = SYS_STATUS_UNINITIALIZED;

        /* Remove the driver usage */
        hObj->objInUse  = false;

        hObj->pPhyObj = 0;
    }

} 


SYS_STATUS DRV_ETHPHY_Status( SYS_MODULE_OBJ hSysObj )
{
    DRV_ETHPHY_OBJ * hObj = (DRV_ETHPHY_OBJ *) hSysObj ;

    /* Check for the valid driver object passed */
    if( NULL != hObj)
    {
        /* Return the status associated with the driver handle */
        return( hObj->status ) ;
    }

    return SYS_STATUS_ERROR;
} 


void DRV_ETHPHY_Tasks( SYS_MODULE_OBJ hSysObj )
{
    DRV_ETHPHY_OBJ * hObj = (DRV_ETHPHY_OBJ *) hSysObj ;

    /* Check for the valid driver object passed */
    if( NULL !=  hObj)
    {
        int ix;
        DRV_ETHPHY_CLIENT_OBJ* hClientObj = hObj->objClients;

        for(ix = 0; ix < sizeof(hObj->objClients) / sizeof(*hObj->objClients); ix++, hClientObj++)
        {
            if(hClientObj->clientInUse != 0)
            {   // active client
                (*_DRV_PHY_ClientOpTbl[hClientObj->operType])(hClientObj);
            }
        }
    }
} 


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************


DRV_HANDLE  DRV_ETHPHY_Open ( const SYS_MODULE_INDEX iModule,
                              const DRV_IO_INTENT ioIntent )
{
    /* Multi client variables are removed from single client builds. */
    DRV_ETHPHY_OBJ * hPhyObj;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj;
    int  clientIx;

    /* Validate the driver index */
    /* If there is anything specific to the module & needs to be checked, should
       be handled in this section with an || condition.
       May be something like ioIntent test for Exclusive access */
    if ( iModule >= _DRV_ETHPHY_INSTANCES_NUMBER || gDrvETHPHYObj[iModule].objInUse == false )
    {
        return DRV_HANDLE_INVALID;
    }

    /* Setup client operations */

    hPhyObj = gDrvETHPHYObj + iModule;
    /* Allocate the client object and set the flag as in use */
    hClientObj = _DRV_ETHPHY_ClientObjectAllocate(hPhyObj, &clientIx) ;
    while(hClientObj)
    {
        DRV_HANDLE miimHandle = hPhyObj->pMiimBase->DRV_MIIM_Open(hPhyObj->miimIndex, DRV_IO_INTENT_SHARED);
        if(miimHandle == DRV_HANDLE_INVALID)
        {
            _PhyDebugCond(false, __func__, __LINE__);
            break;
        }
        hClientObj->pMiimBase = hPhyObj->pMiimBase;
        hClientObj->miimHandle = miimHandle;
        hClientObj->miimOpHandle = 0;

        hClientObj->clientInUse    = true;
        hClientObj->clientIx = clientIx;
        hClientObj->hDriver  = hPhyObj;
        hClientObj->ethphyId = hPhyObj->ethphyId;
        hClientObj->operType = DRV_ETHPHY_CLIENT_OP_TYPE_NONE;
        hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
        _DRV_PHY_SetOperPhase(hClientObj, 0, 0);
        hPhyObj->numClients++;

        /* Update the Client Status */
        hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_READY;

        /* Return the client object */
        return ( ( DRV_HANDLE ) hClientObj );
    }

    return DRV_HANDLE_INVALID;

}


//******************************************************************************
/* Function:
    void DRV_ETHPHY_Close( DRV_HANDLE handle )

  Summary:
    Closes an opened-instance of a driver

  Description:
    This routine closes an opened-instance of a driver, invalidating the given
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None
*/

void DRV_ETHPHY_Close( DRV_HANDLE handle )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *)handle;

    if(hClientObj != 0)
    {
        hClientObj->pMiimBase->DRV_MIIM_Close(hClientObj->miimHandle);

        /* Free the Client Instance */
        hClientObj->clientInUse = false ;
        hClientObj->hDriver->numClients--;

        /* Update the Client Status */
        hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_CLOSED;
    }

}

DRV_ETHPHY_RESULT DRV_ETHPHY_Setup( DRV_HANDLE handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
{

    DRV_ETHPHY_OBJ * hDriver;
    const DRV_ETHPHY_OBJECT* pPhyObj;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    // basic sanity check
    if(pSetUp == 0 || pSetupFlags == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    // check the PHY set up parameters
    hDriver = hClientObj->hDriver;
    pPhyObj = hDriver->pPhyObj;

    if(pPhyObj == 0 || pPhyObj->miiConfigure == 0 || pPhyObj->mdixConfigure == 0 || pPhyObj->smiClockGet == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if(pSetUp->resetFunction)
    {
        (*pSetUp->resetFunction)(gDrvEthBaseObj);
    }
    DRV_MIIM_SETUP  miimSetup;

	miimSetup.hostClockFreq = (uint32_t)TCPIP_INTMAC_PERIPHERAL_CLK;


    miimSetup.maxBusFreq = (*pPhyObj->smiClockGet)(gDrvEthBaseObj, handle);
    miimSetup.setupFlags = DRV_MIIM_SETUP_FLAG_NONE;

    DRV_MIIM_RESULT miimRes = hClientObj->pMiimBase->DRV_MIIM_Setup(hClientObj->miimHandle, &miimSetup); 
    if(miimRes < 0)
    {   // error occurred
        _PhyDebugCond(false, __func__, __LINE__);
        return DRV_ETHPHY_RES_MIIM_ERR;
    }

    hDriver->phyAddress = pSetUp->phyAddress;
    hDriver->openFlags = pSetUp->openFlags;
    hDriver->configFlags = pSetUp->configFlags;
    hDriver->macPauseType = pSetUp->macPauseType;
    
    hClientObj->operParam = (uintptr_t)pSetupFlags;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_SETUP, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}

static void _DRV_ETHPHY_ClientOpNone(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
}

static void _DRV_ETHPHY_ClientOpSetup(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if((unsigned int)hClientObj->operPhase >= sizeof(_DRV_PHY_SetupPhasesTbl)/sizeof(*_DRV_PHY_SetupPhasesTbl))
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
    else
    {
        (*_DRV_PHY_SetupPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
}


static void _DRV_ETHPHY_SetupPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    DRV_ETHPHY_OBJ * hDriver = NULL;
    DRV_ETHPHY_CONFIG_FLAGS configFlags = DRV_ETHPHY_CFG_DEFAULT;
    TCPIP_ETH_OPEN_FLAGS      openFlags = TCPIP_ETH_OPEN_DEFAULT;      // flags required at open time

    hDriver = hClientObj->hDriver;

    configFlags = hDriver->configFlags;

#if defined(__PIC32MX__) || defined(__PIC32MZ__)
    // get the way the hw is configured
    DRV_ETHPHY_CONFIG_FLAGS hwFlags = DRV_ETHPHY_CFG_DEFAULT;
    DRV_ETHPHY_HWConfigFlagsGet((DRV_HANDLE)hClientObj, &hwFlags);

    if(configFlags & DRV_ETHPHY_CFG_AUTO)
    {
        hDriver->configFlags = configFlags = hwFlags;
    }
    else
    {   // some minimal check against the way the hw is configured
        DRV_ETHPHY_CONFIG_FLAGS swFlags = configFlags & (DRV_ETHPHY_CFG_RMII|DRV_ETHPHY_CFG_ALTERNATE);

        if((swFlags ^ hwFlags) != 0)
        {   // hw-sw configuration mismatch MII/RMII, ALT/DEF config
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_CFG_ERR);
        }
    }
#endif

    openFlags = hDriver->openFlags;
    if(openFlags & (TCPIP_ETH_OPEN_PHY_LOOPBACK | TCPIP_ETH_OPEN_MAC_LOOPBACK))
    {
        openFlags &= ~TCPIP_ETH_OPEN_AUTO; // no negotiation in loopback mode!
    }

    if(!(openFlags & TCPIP_ETH_OPEN_AUTO))
    {
        openFlags &= ~TCPIP_ETH_OPEN_MDIX_AUTO;        // Auto-MDIX has to be in auto negotiation only
    }

    openFlags |= (configFlags & DRV_ETHPHY_CFG_RMII) ? TCPIP_ETH_OPEN_RMII : TCPIP_ETH_OPEN_MII;

    hDriver->openFlags = openFlags;

    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 0);
}

    
static void _DRV_ETHPHY_SetupPhaseDetect(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_VENDOR_DETECT detectF;
    DRV_ETHPHY_OBJ* hDriver;
    DRV_ETHPHY_RESULT res; 


    switch (hClientObj->operSubPhase)
    {
        case 0:
            // set the function to perform the transfer
            hDriver = hClientObj->hDriver;

            if((detectF = hDriver->pPhyObj->phyDetect) == 0)
            {   // call the default detect procedure
                detectF = _DRV_ETHPHY_DefaultDetect;
            } 

            hClientObj->vendorDetect = detectF;
            hClientObj->vendorData = 0;
            hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_RESET_CLR_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 1);
            break;

        case 1:
            // perform the transfer
            res =  hClientObj->vendorDetect(gDrvEthBaseObj, (DRV_HANDLE)hClientObj);
            if(res == DRV_ETHPHY_RES_OK)
            {   // everything seems to be fine
                // initiate the PHY reset
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 0);
            }
            else if(res < 0)
            {   // failed 
                _DRV_PHY_SetOperDoneResult(hClientObj, res);
            }
            else
            {   // DRV_ETHPHY_RES_PENDING wait some more...
                if(SYS_TMR_TickCountGet() >= hClientObj->operTStamp)
                {   // tmo occurred
                    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_TMO);
                }
            }
            break;

        default:
            // shouldn't happen
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }

}


// default PHY detection procedure
static DRV_ETHPHY_RESULT _DRV_ETHPHY_DefaultDetect( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE hClientObj)
{
    union
    {
        uint32_t    w;
        struct
        {
            uint16_t low;
            uint16_t high;
        };
    }vendorData;

    __BMCONbits_t bmcon;
    uint16_t    phyReg = 0;
    uint16_t    detectPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &vendorData.w);

    if(res < 0)
    {   // some error occurred
        return res;
    }

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    detectPhase = vendorData.low;

    _PhyDetectPhase(detectPhase);

    // try to detect the PHY and reset it

    switch (detectPhase)
    {
        case 0:
            // initiate a read of the BMCON register
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // advance to the next phase
            vendorData.low = ++detectPhase;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;


        case 1:
            // wait the BMCON read to complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got BMCON result
            bmcon.w =  phyReg;
            _PhyDetectValue(bmcon.w, false);
            if(bmcon.RESET == 0)
            {   // all good;advance to the next phase
                vendorData.low = ++detectPhase;
                pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
                return DRV_ETHPHY_RES_PENDING;
            }

            // that is suspicios...but give it a chance to clear itself
            // otherwise operation will timeout
            // back from start: re-read BMCON
            vendorData.w = 0;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 2:
            // try to see if we can write smth to the PHY
            // we use BMCON::Loopback and duplex bits
            bmcon.w = _BMCON_LOOPBACK_MASK | _BMCON_DUPLEX_MASK;
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, bmcon.w, phyAddress);
            _PhyDetectValue(bmcon.w, true);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }
            vendorData.low = ++detectPhase;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 3:
            // initiate re-read the BMCON
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // advance to the next phase
            vendorData.low = ++detectPhase;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 4:
            // wait the BMCON read to complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            bmcon.w = phyReg;
            _PhyDetectValue(bmcon.w, false);
            if( (bmcon.LOOPBACK == 0) || (bmcon.DUPLEX == 0) )
            {   // failed to set
                return DRV_ETHPHY_RES_DTCT_ERR; 
            }
            // advance to the next phase
            vendorData.high = phyReg;
            vendorData.low = ++detectPhase;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 5:
            // clear bits and write
            bmcon.w = vendorData.high;
            bmcon.w ^= _BMCON_LOOPBACK_MASK | _BMCON_DUPLEX_MASK;
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, bmcon.w, phyAddress);
            _PhyDetectValue(bmcon.w, true);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }
            // advance to the next phase
            vendorData.low = ++detectPhase;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 6:
            // re-read BMCON
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }
            // advance to the next phase
            vendorData.low = ++detectPhase;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 7:
            // wait read complete
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            bmcon.w = phyReg;
            _PhyDetectValue(bmcon.w, false);
            if(bmcon.LOOPBACK || bmcon.DUPLEX)
            {   // failed to clear
                return DRV_ETHPHY_RES_DTCT_ERR;
            }

            // everything seems to be fine
            return DRV_ETHPHY_RES_OK;


        default:
            // shouldn't happen
            return DRV_ETHPHY_RES_OPERATION_ERR;
    }
}

// performs the PHY reset
static void _DRV_ETHPHY_SetupPhaseReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t bmcon;

    switch(hClientObj->operSubPhase)
    {
        case 0:
            // initiate Soft Reset the PHY
            if(_DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, _BMCON_RESET_MASK))
            {
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 1);
            }
            break;

        case 1:
            // read the BMCON
            if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON))
            {
                hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_RESET_CLR_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 2);
            }
            break;

        case 2:
            // wait the BMCON write/read to complete
            if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
            {
                break;
            }

            bmcon.w = hClientObj->smiData;
            if(bmcon.RESET)
            {   // wait to clear itself
                if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
                {   // not yet tmo; re-read
                    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 1);
                }
                else
                {
                    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
                }
            }
            else
            {   // success
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 0);
            }
            break;

        default:
            // shouldn't happen
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
            break;
    }

}

static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase0(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase1(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase2(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase3(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase4(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase5(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase6(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase7(DRV_ETHPHY_CLIENT_OBJ * hClientObj);




static const _DRV_PHY_OperPhaseF _DRV_PHY_SetupPhaseNegotiateSubPhaseTbl[] = 
{
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase0,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase1,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase2,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase3,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase4,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase5,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase6,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase7,


};

static void _DRV_ETHPHY_SetupPhaseNegotiate(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(hClientObj->operSubPhase <  sizeof(_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl) / sizeof(*_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl))
    {
        (*_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl[hClientObj->operSubPhase])(hClientObj);
    } 
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase0(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    TCPIP_ETH_OPEN_FLAGS  openFlags;      // flags required at open time
    uint16_t  openReqs;
    
    // provide some defaults
    openFlags = hClientObj->hDriver->openFlags;
    if(!(openFlags & (TCPIP_ETH_OPEN_FDUPLEX|TCPIP_ETH_OPEN_HDUPLEX)))
    {
        openFlags |= TCPIP_ETH_OPEN_HDUPLEX;
    }
    if(!(openFlags & (TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_10)))
    {
        openFlags |= TCPIP_ETH_OPEN_10;
    }

    if(openFlags & TCPIP_ETH_OPEN_AUTO)
    {   // advertise auto negotiation
        openReqs = _BMSTAT_AN_ABLE_MASK;

        if(openFlags & TCPIP_ETH_OPEN_100)
        {
            if(openFlags & TCPIP_ETH_OPEN_FDUPLEX)
            {
                openReqs |= _BMSTAT_BASE100TX_FDX_MASK;
            }
            if(openFlags & TCPIP_ETH_OPEN_HDUPLEX)
            {
                openReqs |= _BMSTAT_BASE100TX_HDX_MASK;
            }
        }

        if(openFlags & TCPIP_ETH_OPEN_10)
        {
            if(openFlags & TCPIP_ETH_OPEN_FDUPLEX)
            {
                openReqs |= _BMSTAT_BASE10T_FDX_MASK;
            }
            if(openFlags & TCPIP_ETH_OPEN_HDUPLEX)
            {
                openReqs |= _BMSTAT_BASE10T_HDX_MASK;
            }
        }
    }
    else
    {   // no auto negotiation
        if(openFlags & TCPIP_ETH_OPEN_100)
        {
            openReqs = (openFlags & TCPIP_ETH_OPEN_FDUPLEX) ? _BMSTAT_BASE100TX_FDX_MASK : _BMSTAT_BASE100TX_HDX_MASK;
        }
        else
        {
            openReqs = (openFlags & TCPIP_ETH_OPEN_FDUPLEX) ? _BMSTAT_BASE10T_FDX_MASK : _BMSTAT_BASE10T_HDX_MASK;
        }
    }

    hClientObj->hDriver->openFlags = openFlags;
    hClientObj->operReg[0] = openReqs;
    // try to match the openFlags with the PHY capabilities
    // initiate the read of the PHY capabilities
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 1);
    }

}

static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase1(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the BMCON PHY_REG_BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    uint16_t  phyCpbl, matchCpbl;

    phyCpbl   = hClientObj->smiData;

    matchCpbl = (hClientObj->operReg[0] & (MAC_COMM_CPBL_MASK | _BMSTAT_AN_ABLE_MASK)) & phyCpbl; // common features
    if(!(matchCpbl & MAC_COMM_CPBL_MASK))
    {   // no match?
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_CPBL_ERR);
        return;
    }

    // we're ok, we can configure the PHY
    hClientObj->operReg[1] = matchCpbl;
    hClientObj->vendorData = 0;
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 2);

}

// MII/RMII configuration phase
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase2(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_OBJ *hDriver;
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;

    hDriver = hClientObj->hDriver;
    res = (*hDriver->pPhyObj->miiConfigure)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj, hDriver->configFlags);

    if(res == DRV_ETHPHY_RES_OK)
    {   // success, advance
        // next phase: configure the MDIX
        hClientObj->vendorData = 0;
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 3);
    }
    else if(res != DRV_ETHPHY_RES_PENDING)
    {   // some error occurred
        _DRV_PHY_SetOperDoneResult(hClientObj, res);
    }

}


// MDIX configure
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase3(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_RESULT res;
    DRV_ETHPHY_OBJ *hDriver = hClientObj->hDriver;

    res = (*hDriver->pPhyObj->mdixConfigure)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj, hClientObj->hDriver->openFlags);

    if(res < 0)
    {   // some error occurred
        _DRV_PHY_SetOperDoneResult(hClientObj, res);
        return;
    }
    else if(res != DRV_ETHPHY_RES_OK)
    {   // still waiting
        return;
    }


    // success, advance;
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 4);
}


static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase4(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_OBJ *hDriver = hClientObj->hDriver;

    // restore match capabilities
    uint16_t  matchCpbl = hClientObj->operReg[1];

    if(matchCpbl &_BMSTAT_AN_ABLE_MASK)
    {   // ok, we can perform auto negotiation
        uint16_t anadReg;

        anadReg = (((matchCpbl >> _BMSTAT_NEGOTIATION_POS) << _ANAD_NEGOTIATION_POS) & _ANAD_NEGOTIATION_MASK) | PROT_802_3;
        if(hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_PAUSE)
        {
            anadReg |= _ANAD_PAUSE_MASK;
        }

        if(hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_ASM_DIR)
        {
            anadReg |= _ANAD_ASM_DIR_MASK;
        }

        if(_DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_ANAD, anadReg))
        {   // advertise our capabilities
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 5);
        }
    }
    else
    {   // ok, just don't use negotiation
        uint16_t  ctrlReg;

        ctrlReg = 0;
        if(matchCpbl & (_BMSTAT_BASE100TX_HDX_MASK | _BMSTAT_BASE100TX_FDX_MASK))   // set 100Mbps request/capability
        {
            ctrlReg |= _BMCON_SPEED_MASK;
        }

        if(matchCpbl & (_BMSTAT_BASE10T_FDX_MASK | _BMSTAT_BASE100TX_FDX_MASK))
        {
            ctrlReg |= _BMCON_DUPLEX_MASK;
        }

        if(hClientObj->hDriver->openFlags & TCPIP_ETH_OPEN_PHY_LOOPBACK)
        {
            ctrlReg |= _BMCON_LOOPBACK_MASK;
        }

        if(_DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, ctrlReg))
        {   // update the configuration
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 7);
        }
    }

}

// advertise, negotiation restart phase
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase5(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
    {   // read capabilities initiate
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 6);
    }
}
    
// auto-negotiation perform
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase6(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the PHY_REG_BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    __BMSTATbits_t  phyCpbl;

    phyCpbl.w = hClientObj->smiData;

    if(phyCpbl.AN_ABLE)
    {   // ok, we can perform auto negotiation
        // restart negotiation and we'll have to wait
        if(_DRV_PHY_SMIWriteStart( hClientObj, PHY_REG_BMCON, _BMCON_AN_ENABLE_MASK | _BMCON_AN_RESTART_MASK ))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 7);
        }
    }
    else
    {   // no negotiation ability!
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_UNABLE);
    }
}

// final update stage
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase7(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    TCPIP_ETH_OPEN_FLAGS openFlags = TCPIP_ETH_OPEN_DEFAULT;
    uint16_t    matchCpbl = 0 ;

    // now update the open flags
    // the upper layer needs to know the PHY set-up to further set-up the MAC.
    openFlags = hClientObj->hDriver->openFlags;
    matchCpbl = hClientObj->operReg[1];

    // clear the capabilities
    openFlags &= ~(TCPIP_ETH_OPEN_AUTO | TCPIP_ETH_OPEN_FDUPLEX | TCPIP_ETH_OPEN_HDUPLEX | TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_10);

    if(matchCpbl & _BMSTAT_AN_ABLE_MASK)
    {
        openFlags |= TCPIP_ETH_OPEN_AUTO;
    }
    if(matchCpbl & (_BMSTAT_BASE100TX_HDX_MASK | _BMSTAT_BASE100TX_FDX_MASK))   // set 100Mbps request/capability
    {
        openFlags |= TCPIP_ETH_OPEN_100;
    }
    if(matchCpbl & (_BMSTAT_BASE10T_HDX_MASK | _BMSTAT_BASE10T_FDX_MASK))   // set 10Mbps request/capability
    {
        openFlags |= TCPIP_ETH_OPEN_10;
    }
    if(matchCpbl & (_BMSTAT_BASE10T_FDX_MASK | _BMSTAT_BASE100TX_FDX_MASK))
    {
        openFlags |= TCPIP_ETH_OPEN_FDUPLEX;
    }
    if(matchCpbl & (_BMSTAT_BASE10T_HDX_MASK | _BMSTAT_BASE100TX_HDX_MASK))
    {
        openFlags |= TCPIP_ETH_OPEN_HDUPLEX;
    }

    // store the openFlags!
    // upper layer needs to know the PHY set-up to further set-up the MAC.
    hClientObj->hDriver->openFlags = openFlags;

    // we're done
    // deposit result
    TCPIP_ETH_OPEN_FLAGS* pSetUpFlags = (TCPIP_ETH_OPEN_FLAGS*)hClientObj->operParam;
    *pSetUpFlags = openFlags;
    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);

} 



DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_ClientStatus( DRV_HANDLE handle )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( NULL != hClientObj)
    {
        /* Return the client status associated with the handle passed */
        return( hClientObj->status );
    }

    return DRV_ETHPHY_CLIENT_STATUS_ERROR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationResult( DRV_HANDLE handle)  
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( NULL != hClientObj)
    {
        /* Return the client status associated with the handle passed */
        return( hClientObj->operRes );
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationAbort( DRV_HANDLE handle)  
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( NULL != hClientObj)
    {
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_ABORTED);
        return DRV_ETHPHY_RES_OK;
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - SMI/MIIM Interface
// *****************************************************************************
// *****************************************************************************

DRV_ETHPHY_RESULT DRV_ETHPHY_PhyAddressGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, int* pPhyAddress)
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(pPhyAddress)
    {
        *pPhyAddress = hClientObj->hDriver->phyAddress;
    }
    return DRV_ETHPHY_RES_OK;
}


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client & Module Level
// *****************************************************************************
// *****************************************************************************

DRV_ETHPHY_RESULT DRV_ETHPHY_RestartNegotiation( DRV_HANDLE hClient, DRV_ETHPHY_INTERFACE_INDEX portIndex )
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) hClient;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESTART, DRV_ETHPHY_RES_PENDING);
    return DRV_ETHPHY_RES_PENDING;
}

static void _DRV_ETHPHY_ClientOpNegRestart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegRestartPhasesTbl)/sizeof(*_DRV_PHY_NegRestartPhasesTbl))
    {
        (*_DRV_PHY_NegRestartPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_NegRestartPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the capabilities control reg: BMSTAT
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESTART_PHASE_READ, 0);
    }
}

static void _DRV_ETHPHY_NegRestartPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMSTATbits_t  phyCpbl;

    // wait the BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyCpbl.w = hClientObj->smiData;
    if(phyCpbl.AN_ABLE == 0)
    {   // no negotiation ability!
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_UNABLE);
        return;
    }

    // we can perform auto negotiation
    // restart negotiation and we'll have to wait
    if(_DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, _BMCON_AN_ENABLE_MASK | _BMCON_AN_RESTART_MASK ))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESTART_PHASE_WRITE, 0);
    }
}

static void _DRV_ETHPHY_NegRestartPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}

DRV_ETHPHY_RESULT DRV_ETHPHY_HWConfigFlagsGet( DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    DRV_ETHPHY_CONFIG_FLAGS hwFlags;
    DRV_ETHPHY_RESULT ethRes;
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    // get the way the hw is configured
#if defined(TCPIP_IF_PIC32MZW_ETHMAC)
    hwFlags = DRV_ETHPHY_CFG_RMII;
    ethRes = DRV_ETHPHY_RES_OK;
#elif defined (TCPIP_IF_ETHMAC)
    hwFlags =  (DEVCFG3bits.FMIIEN != 0) ?     DRV_ETHPHY_CFG_MII : DRV_ETHPHY_CFG_RMII;
    hwFlags |= (DEVCFG3bits.FETHIO != 0) ? DRV_ETHPHY_CFG_DEFAULT : DRV_ETHPHY_CFG_ALTERNATE;
    ethRes = DRV_ETHPHY_RES_OK;
#elif defined (TCPIP_IF_GMAC)
    hwFlags = ((GMAC_REGS->GMAC_UR & GMAC_UR_Msk)== DRV_GMAC_RMII_MODE) ?  DRV_ETHPHY_CFG_RMII : DRV_ETHPHY_CFG_MII;
    ethRes = DRV_ETHPHY_RES_OK;
#elif defined(TCPIP_IF_EMAC0) || defined(TCPIP_IF_EMAC1)
    hwFlags = DRV_ETHPHY_CFG_RMII;
    ethRes = DRV_ETHPHY_RES_OK;
#else
    hwFlags = 0;
    ethRes = DRV_ETHPHY_RES_CFG_ERR;
#endif

    if(pFlags)
    {

        *pFlags = hwFlags;
    }

    return ethRes;
}


DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationIsComplete( DRV_HANDLE hClient, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete )
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) hClient;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    hClientObj->operParam = waitComplete;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_COMPLETE, DRV_ETHPHY_RES_PENDING);


    return DRV_ETHPHY_RES_PENDING;
}

static void _DRV_ETHPHY_ClientOpNegComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegCompletePhasesTbl)/sizeof(*_DRV_PHY_NegCompletePhasesTbl))
    {
        (*_DRV_PHY_NegCompletePhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_NegCompletePhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the control reg: PHY_REG_BMCON
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_READ1, 0);
    }
}

static void _DRV_ETHPHY_NegCompletePhaseRead1(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;
    // wait the BMCON read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyBMCon.w = hClientObj->smiData;
    if(phyBMCon.AN_ENABLE == 0)
    {   // no negotiation is taking place; we're done
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_INACTIVE);
        return;
    }

    // re-issue another BMCON read
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_READ2, 0);
    }
}

static void _DRV_ETHPHY_NegCompletePhaseRead2(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the BMCON read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save BMCON read for later
    hClientObj->operReg[1] = hClientObj->smiData;

    if(hClientObj->operParam == 0)
    {   // no wait complete
        // issue a PHY_REG_BMSTAT read
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_RESULT_NOWAIT, 0);
        }
        return;
    }

    // wait for negotiation completion
    // and launch another BMCON read op
    hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_NEG_INIT_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_AN_RESTART, 0);
    }
}

static void _DRV_ETHPHY_NegCompletePhaseAN_Restart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;

    // wait the BMCON read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save BMCON read for later
    hClientObj->operReg[1] = phyBMCon.w = hClientObj->smiData;

    if(phyBMCon.AN_RESTART == 0)
    {   // negotiation started; wait to complete
        // and launch another BMSTAT read op
        hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_NEG_DONE_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_AN_COMPLETE, 0);
        }
        return;
    }

    if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
    {   // wait some more; re-read
        _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
    }
    else
    {   // timeout
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED);
    }
}

static void _DRV_ETHPHY_NegCompletePhaseAN_Complete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;
    __BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyStat.w = hClientObj->smiData;

    if(phyStat.AN_COMPLETE == 0)
    {   // not done yet
        if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
        {   // wait some more
            _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
        }
        else
        {   // timeout
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
        }
    }
    else
    {
        phyBMCon.w = hClientObj->operReg[1];   // restore BMCON read
        _DRV_PHY_NegCompleteSetOperResult(hClientObj, phyBMCon.w, phyStat.w);
    }

}

static void _DRV_ETHPHY_NegCompletePhaseResultNoWait(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;
    __BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyStat.w = hClientObj->smiData;
    phyBMCon.w = hClientObj->operReg[1];   // restore BMCON read

    _DRV_PHY_NegCompleteSetOperResult(hClientObj, phyBMCon.w, phyStat.w);
}

static void _DRV_PHY_NegCompleteSetOperResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t phyBmConReg, uint16_t phyStatReg)
{
    DRV_ETHPHY_RESULT res;
    __BMCONbits_t   phyBMCon;
    __BMSTATbits_t  phyStat;

    phyBMCon.w = phyBmConReg;
    phyStat.w = phyStatReg;

    if(!phyBMCon.AN_ENABLE)
    {
        res = DRV_ETHPHY_RES_NEGOTIATION_INACTIVE;       // no negotiation is taking place!
    }
    else if(phyBMCon.AN_RESTART)
    {
        res = DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED;        // not started yet/tmo
    }
    else
    {
        res = (phyStat.AN_COMPLETE == 0) ? DRV_ETHPHY_RES_NEGOTIATION_ACTIVE : DRV_ETHPHY_RES_OK;    // active/tmo/ok
    }

    _DRV_PHY_SetOperDoneResult(hClientObj, res);
}



DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    // basic sanity check
    if(pNegResult == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    hClientObj->operParam = (uintptr_t)pNegResult;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESULT, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}


static void _DRV_ETHPHY_ClientOpNegResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegResultPhasesTbl)/sizeof(*_DRV_PHY_NegResultPhasesTbl))
    {
        (*_DRV_PHY_NegResultPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_NegResultPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the control reg: PHY_REG_BMSTAT
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_BMSTAT, 0);
    }
}

static void _DRV_ETHPHY_NegResultPhase_BMSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMSTATbits_t   phyStat;
    // wait the BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyStat.w = hClientObj->smiData;
    if(phyStat.AN_COMPLETE == 0)
    {   // negotiation is not yet done; we're finished
        _DRV_PHY_NegResultSetOperResult(hClientObj, ( DRV_ETHPHY_LINK_ST_DOWN | DRV_ETHPHY_LINK_ST_NEG_TMO), 0, TCPIP_ETH_PAUSE_TYPE_NONE, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
    }
    else if(phyStat.LINK_STAT == 0)
    {
        _DRV_PHY_NegResultSetOperResult(hClientObj, DRV_ETHPHY_LINK_ST_DOWN, 0, TCPIP_ETH_PAUSE_TYPE_NONE, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
    }
    else
    {   // we're up and running
        // read the control reg: ANEXP
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_ANEXP))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_ANEXP, 0);
        }
    }
}


static void _DRV_ETHPHY_NegResultPhase_ANEXP(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the ANEXP read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save the ANEXP for later
    hClientObj->operReg[0] = hClientObj->smiData;

    // read the control reg: ANLPAD
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_ANLPAD))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_ANLPAD, 0);
    }

}

static void _DRV_ETHPHY_NegResultPhase_ANLPAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the ANLPAD read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save the ANLPAD for later
    hClientObj->operReg[1] = hClientObj->smiData;

    // read the control reg: ANAD
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_ANAD))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_ANAD, 0);
    }
}

static void _DRV_ETHPHY_NegResultPhase_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __ANEXPbits_t    phyExp;
    __ANLPADbits_t   lpAD;
    __ANADbits_t     anadReg;
    int              lcl_Pause, lcl_AsmDir, lp_Pause, lp_AsmDir;     // pause capabilities, local and LP
    DRV_ETHPHY_LINK_STATUS  linkStat;
    TCPIP_ETH_OPEN_FLAGS   oFlags;
    TCPIP_ETH_PAUSE_TYPE   pauseType;

    // wait the ANAD read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    anadReg.w = hClientObj->smiData;
    // restore ANEXP
    phyExp.w = hClientObj->operReg[0];
    // restore ANLPAD
    lpAD.w = hClientObj->operReg[1];


    oFlags = 0;   // don't know the result yet
    pauseType = TCPIP_ETH_PAUSE_TYPE_NONE;
    linkStat = DRV_ETHPHY_LINK_ST_UP;

    lcl_Pause  = (hClientObj->hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_PAUSE) ? 1 : 0;
    lcl_AsmDir = (hClientObj->hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_ASM_DIR) ? 1 : 0;
    lp_Pause   = lp_AsmDir = 0;         // in case negotiation failed

    if(phyExp.LP_AN_ABLE)
    {   // ok,valid auto negotiation info

        if(lpAD.REM_FAULT)
        {
            linkStat |= DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
        }

        if(lpAD.PAUSE)
        {
            linkStat |= DRV_ETHPHY_LINK_ST_LP_PAUSE;
            lp_Pause = 1;
        }
        if(lpAD.ASM_DIR)
        {
            linkStat |= DRV_ETHPHY_LINK_ST_LP_ASM_DIR;
            lp_AsmDir = 1;
        }
    }
    else
    {   // even if negotiation is not enabled, the link partner advertising shows the configuration
        linkStat |= DRV_ETHPHY_LINK_ST_LP_NEG_UNABLE;
        if(phyExp.PDF)
        {
            linkStat |= DRV_ETHPHY_LINK_ST_PDF;
        }
    }

    // set the PHY connection params
    // advertised capabilities & the matching ones
    anadReg.w &= lpAD.w;
    // get the settings, according to IEEE 802.3 Annex 28B.3 Priority Resolution
    // Note: we don't support 100BaseT4 !

    if(anadReg.w & _ANAD_BASE100TX_FDX_MASK)
    {
        oFlags = (TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_FDUPLEX);
    }
    else if(anadReg.w & _ANAD_BASE100TX_MASK)
    {
        oFlags = (TCPIP_ETH_OPEN_100 | TCPIP_ETH_OPEN_HDUPLEX);
    }
    else if(anadReg.w & _ANAD_BASE10T_FDX_MASK)
    {
        oFlags = (TCPIP_ETH_OPEN_10 | TCPIP_ETH_OPEN_FDUPLEX);
    }
    else if(anadReg.w & _ANAD_BASE10T_MASK)
    {
        oFlags = (TCPIP_ETH_OPEN_10 | TCPIP_ETH_OPEN_HDUPLEX);
    }
    else
    {   // this should NOT happen!
        linkStat |= DRV_ETHPHY_LINK_ST_NEG_FATAL_ERR;
        linkStat &= ~DRV_ETHPHY_LINK_ST_UP;      // make sure we stop...!
    }

    // set the pause type for the MAC
    // according to IEEE Std 802.3-2002 Tables 28B-2, 28B-3
    if(oFlags & TCPIP_ETH_OPEN_FDUPLEX)
    {   // pause type relevant for full duplex only
        if(lp_Pause & (lcl_Pause | (lcl_AsmDir & lp_AsmDir)))
        {
            pauseType = TCPIP_ETH_PAUSE_TYPE_EN_TX;
        }
        if(lcl_Pause & (lp_Pause | (lcl_AsmDir & lp_AsmDir)))
        {
            pauseType |= TCPIP_ETH_PAUSE_TYPE_EN_RX;
        }
    }

    _DRV_PHY_NegResultSetOperResult(hClientObj, linkStat, oFlags, pauseType, DRV_ETHPHY_RES_OK);
}


static void _DRV_PHY_NegResultSetOperResult(DRV_ETHPHY_CLIENT_OBJ* hClientObj, DRV_ETHPHY_LINK_STATUS linkStatus,
       TCPIP_ETH_OPEN_FLAGS openFlags, TCPIP_ETH_PAUSE_TYPE pauseType, DRV_ETHPHY_RESULT res)
{
    DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult = (DRV_ETHPHY_NEGOTIATION_RESULT*)hClientObj->operParam;

    if(pNegResult)
    {
        pNegResult->linkStatus = linkStatus;
        pNegResult->linkFlags = openFlags;
        pNegResult->pauseType = pauseType;
    }

    _DRV_PHY_SetOperDoneResult(hClientObj, res);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }


    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    // basic sanity check
    if(pLinkStat == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    hClientObj->operParam = (uintptr_t)pLinkStat;
    hClientObj->operReg[0] = refresh;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_LINK_STAT, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}
 
static void _DRV_ETHPHY_ClientOpLinkStat(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_LinkStatPhasesTbl)/sizeof(*_DRV_PHY_LinkStatPhasesTbl))
    {
        (*_DRV_PHY_LinkStatPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_LinkStatPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the link status: PHY_REG_BMSTAT
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_LINK_STAT_PHASE_READ, 0);
    }
}

// read from PHY_REG_BMSTAT
static void _DRV_ETHPHY_LinkStatPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // read the link status
    phyStat.w = hClientObj->smiData;
    if( phyStat.LINK_STAT != 0 || hClientObj->operReg[0] == 0)
    {   // link up or no refresh needed
        DRV_ETHPHY_LINK_STATUS* pLinkStat = (DRV_ETHPHY_LINK_STATUS*)hClientObj->operParam;
        if(pLinkStat)
        {
            *pLinkStat = _Phy2LinkStat(phyStat);
        }
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
    }
    else
    {   // initiate another read
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_LINK_STAT_PHASE_REFRESH, 0);
        }
    }
}

// read from PHY_REG_BMSTAT
static void _DRV_ETHPHY_LinkStatPhaseRefresh(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // read the link status
    phyStat.w = hClientObj->smiData;

    DRV_ETHPHY_LINK_STATUS* pLinkStat = (DRV_ETHPHY_LINK_STATUS*)hClientObj->operParam;
    if(pLinkStat)
    {
        *pLinkStat = _Phy2LinkStat(phyStat);
    }

    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_Reset( DRV_HANDLE handle, bool waitComplete )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    hClientObj->operParam = waitComplete;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_RESET, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}


static void _DRV_ETHPHY_ClientOpReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_ResetPhasesTbl)/sizeof(*_DRV_PHY_ResetPhasesTbl))
    {
        (*_DRV_PHY_ResetPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_ResetPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // initiate Soft Reset of the PHY
    if(_DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, _BMCON_RESET_MASK))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_RESET_PHASE_WRITE, 0);
    }
}

static void _DRV_ETHPHY_ResetPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if( hClientObj->operParam == 0)
    {   // no need to wait, we're done
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
    }
    else
    {   // read the BMCON
        hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_RESET_CLR_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_RESET_PHASE_READ, 0);
    }
}

static void _DRV_ETHPHY_ResetPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t bmcon;

    switch(hClientObj->operSubPhase)
    {
        case 0:
            // initiate BMCON read
            if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON))
            {
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_RESET_PHASE_READ, 1);
            }
            break;

        case 1:
            // wait the BMCON read to complete
            if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
            {
                break;
            }

            bmcon.w = hClientObj->smiData;
            if(bmcon.RESET)
            {   // wait to clear itself
                if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
                {   // not yet tmo; re-read
                    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_RESET_PHASE_READ, 0);
                }
                else
                {   // failed by timeout
                    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
                }
            }
            else
            {   // reset cleared itself; success
                _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
            }
            break;

        default:
            // shouldn't happen
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Vendor interface Routines
// *****************************************************************************
// *****************************************************************************


DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataGet( DRV_HANDLE handle, uint32_t* pVendorData )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(pVendorData)
    {
        *pVendorData = hClientObj->vendorData;
    }

    return DRV_ETHPHY_RES_OK;

}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataSet( DRV_HANDLE handle, uint32_t vendorData )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    hClientObj->vendorData = vendorData;

    return DRV_ETHPHY_RES_OK;

}



DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadStart( DRV_HANDLE handle, uint16_t rIx,  int phyAddress )
{
    DRV_ETHPHY_SMI_TXFER_RES opRes;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    
    if( hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || 
        hClientObj->operType != DRV_ETHPHY_CLIENT_OP_TYPE_SETUP || 
        (hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_DETECT && hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_NEGOTIATE))
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_NONE)
    {   // calling this before the previous SMI done?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }


    opRes = _DRV_PHY_SMIReadStartEx(hClientObj, rIx, phyAddress);
    if(opRes < 0)
    {   // error occurred 
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {
        hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
        return DRV_ETHPHY_RES_PENDING;
    } 

    // went through
    return DRV_ETHPHY_RES_OK;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadResultGet( DRV_HANDLE handle, uint16_t* pSmiRes)
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if( hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || 
        hClientObj->operType != DRV_ETHPHY_CLIENT_OP_TYPE_SETUP || 
        (hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_DETECT && hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_NEGOTIATE))
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE)
    {   // not called as a result of a previous read?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    DRV_ETHPHY_SMI_TXFER_RES opRes = _DRV_PHY_SMITransferWaitComplete(hClientObj);
    if(opRes < 0)
    {   // error
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_WAIT)
    {   // wait some more
        return DRV_ETHPHY_RES_PENDING;
    }
    // done

    if(pSmiRes)
    {
        *pSmiRes = hClientObj->smiData;
    }
    
    return DRV_ETHPHY_RES_OK;

}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIWriteStart( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress )
{
    DRV_ETHPHY_RESULT phyRes;
    DRV_ETHPHY_SMI_TXFER_RES opRes;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    
    if( hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || 
        hClientObj->operType != DRV_ETHPHY_CLIENT_OP_TYPE_SETUP || 
        (hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_DETECT && hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_NEGOTIATE))
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_NONE)
    {   // calling this before the previous SMI done?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    opRes = _DRV_PHY_SMIWriteStartEx(hClientObj, rIx, wData, phyAddress);
    if(opRes < 0)
    {   // error
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {   // retry
        phyRes = DRV_ETHPHY_RES_PENDING;
    }
    else
    {   // went through
        phyRes = DRV_ETHPHY_RES_OK;
    }

    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;


    return phyRes;

}

/*******************************************************************************
End of File
*/
