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
#define _PHY_PROT_802_3         0x01    // IEEE 802.3 capability
// all comm capabilities our MAC supports
#define _PHY_STD_CPBL_MASK      (_BMSTAT_BASE10T_HDX_MASK | _BMSTAT_BASE10T_FDX_MASK |          \
                                _BMSTAT_BASE100TX_HDX_MASK |_BMSTAT_BASE100TX_FDX_MASK |        \
                                _BMSTAT_EXTSTAT_MASK)
                                
#define _PHY_EXT_CPBL_MASK          (_EXTSTAT_1000BASEX_FDX_MASK | _EXTSTAT_1000BASEX_HDX_MASK |    \
                                _EXTSTAT_1000BASET_FDX_MASK | _EXTSTAT_1000BASET_HDX_MASK)


#define _PHY_BMCON_DETECT_MASK  (_BMCON_LOOPBACK_MASK | _BMCON_DUPLEX_MASK) 

// local prototypes
// debug
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC) != 0)
static volatile int _PhyStayAssertLoop = 0;
void _DRV_ETHPHY_AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PHY Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_PhyStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
static volatile int _PhyStayCondLoop = 0;
static void _PhyDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PHY Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_PhyStayCondLoop != 0);
    }
}

#else
#define _PhyDebugCond(cond, message, lineNo)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC)

#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_PHASE) != 0)
void _DRV_ETHPHY_Dbg_DetectPhase(uint16_t detectPhase)
{
    static uint16_t prevState = 0xffff;
    if(detectPhase != prevState)
    {
        prevState = detectPhase;
        SYS_CONSOLE_PRINT("PHY detect phase: %d\r\n", detectPhase);
    }
}
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_PHASE)

#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_VALUES) != 0)
void _DRV_ETHPHY_Dbg_DetectWriteValue(int rIx, uint16_t rVal)
{
    SYS_CONSOLE_PRINT("PHY detect write - reg: %d, val: 0x%04x\r\n", rIx, rVal);
}

void _DRV_ETHPHY_Dbg_DetectReadValue(int rIx, uint16_t rVal, uint16_t valMask, uint16_t chkMask)
{
    uint16_t chkVal = rVal & valMask;   // keep only bits to be checked
    uint16_t xorVal = chkVal ^ chkMask; 
    if(xorVal == 0)
    {
        SYS_CONSOLE_PRINT("PHY detect read match - reg: %d, val: 0x%04x\r\n", rIx, rVal);
    }
    else
    {
        SYS_CONSOLE_PRINT("PHY detect read Fail - reg: %d, got: 0x%04x, fail mask: 0x%04x\r\n", rIx, rVal, xorVal);
    }
}
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
static void _DRV_ETHPHY_SetupPhaseReadId(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate(DRV_ETHPHY_CLIENT_OBJ * hClientObj);



static const _DRV_PHY_OperPhaseF _DRV_PHY_SetupPhasesTbl[] = 
{
    _DRV_ETHPHY_SetupPhaseIdle,
    _DRV_ETHPHY_SetupPhaseDetect,
    _DRV_ETHPHY_SetupPhaseReadId,
    _DRV_ETHPHY_SetupPhaseReset,
    _DRV_ETHPHY_SetupPhaseNegotiate,
};


static void _DRV_ETHPHY_LinkStatPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_LinkStatPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_LinkStatPhaseRefresh(DRV_ETHPHY_CLIENT_OBJ * hClientObj);



static const _DRV_PHY_OperPhaseF _DRV_PHY_LinkStatPhasesTbl[] = 
{
    _DRV_ETHPHY_LinkStatPhaseIdle,      // DRV_ETHPHY_LINK_STAT_PHASE_IDLE
    _DRV_ETHPHY_LinkStatPhaseRead,      // DRV_ETHPHY_LINK_STAT_PHASE_READ
    _DRV_ETHPHY_LinkStatPhaseRefresh,   // DRV_ETHPHY_LINK_STAT_PHASE_REFRESH
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
static void _DRV_ETHPHY_NegResultPhase_EXTSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_1000BASECTRL(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_1000BASESTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANEXP(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANLPAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void _DRV_PHY_NegResultSetOperResult(DRV_ETHPHY_CLIENT_OBJ* hClientObj, DRV_ETHPHY_LINK_STATUS linkStatus,
                                    TCPIP_ETH_OPEN_FLAGS openFlags, TCPIP_ETH_PAUSE_TYPE pauseType, DRV_ETHPHY_RESULT res);

static const _DRV_PHY_OperPhaseF _DRV_PHY_NegResultPhasesTbl[] = 
{
    _DRV_ETHPHY_NegResultPhaseIdle,
    _DRV_ETHPHY_NegResultPhase_BMSTAT,
    _DRV_ETHPHY_NegResultPhase_EXTSTAT,
    _DRV_ETHPHY_NegResultPhase_1000BASECTRL,
    _DRV_ETHPHY_NegResultPhase_1000BASESTAT,
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
//      - < 0 an error occurred;  smiTxferStatus unchanged (should be DRV_ETHPHY_SMI_TXFER_OP_NONE)
//      - DRV_ETHPHY_SMI_TXFER_RES_BUSY: transaction needs to be retried; smiTxferStatus unchanged (should be DRV_ETHPHY_SMI_TXFER_OP_NONE)
//      - DRV_ETHPHY_SMI_TXFER_RES_SCHEDULED: transaction started (read/write complete); smiTxferStatus == DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE
//      - DRV_ETHPHY_SMI_TXFER_RES_DONE: transaction done (simple write); smiTxferStatus == DRV_ETHPHY_SMI_TXFER_OP_NONE
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
        _DRV_ETHPHY_AssertCond(false, __func__, __LINE__);
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
// Note: this should be called only when (hClientObj->smiTxferStatus == DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE
//      i.e. when the SMI _DRV_PHY_SMITransferStart operation has been called with:
//      DRV_ETHPHY_SMI_XFER_TYPE_READ or DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE
//      For a simple DRV_ETHPHY_SMI_XFER_TYPE_WRITE operation, the result is discarded, so no wait is needed!
static DRV_ETHPHY_SMI_TXFER_RES _DRV_PHY_SMITransferWaitComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_SMI_TXFER_RES txferRes;
    DRV_MIIM_RESULT miimRes;
    uint32_t  opData;

    if(hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE)
    {   // nothing to wait for; but some functions may call this if a write operation was failed by MIIM and needs retry
        _PhyDebugCond(false, __func__, __LINE__);
        return DRV_ETHPHY_SMI_TXFER_RES_DONE;
    } 

    if(hClientObj->miimHandle == 0)
    {   // shouldn't happen
        _DRV_ETHPHY_AssertCond(false, __func__, __LINE__);
        return DRV_ETHPHY_SMI_TXFER_RES_ILLEGAL;
    } 

    miimRes = hClientObj->pMiimBase->DRV_MIIM_OperationResult(hClientObj->miimHandle, hClientObj->miimOpHandle, &opData);
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
            hClientObj->smiData = (uint16_t)opData; // set the read result
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
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
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
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
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
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
        return false;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {   // retry
        return false;
    }

    // went through
    return true;
}

static DRV_ETHPHY_SMI_TXFER_RES _DRV_PHY_SMIWriteStartEx(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, uint16_t wData, int phyAdd, bool waitComplete)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = waitComplete ? DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE : DRV_ETHPHY_SMI_XFER_TYPE_WRITE;
    hClientObj->smiData =  wData;
    hClientObj->smiPhyAddress = phyAdd;

   return  _DRV_PHY_SMITransferStart(hClientObj);
}

static void _DRV_PHY_SetOperPhase(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t operPhase, uint16_t operSubPhase)
{
    hClientObj->operPhase = operPhase;
    hClientObj->operSubPhase = operSubPhase;
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

#ifndef TCPIP_STACK_INTMAC_COUNT
    #define _DRV_ETHPHY_INSTANCES_NUMBER    1
#else
    #define _DRV_ETHPHY_INSTANCES_NUMBER    TCPIP_STACK_INTMAC_COUNT
#endif

static DRV_ETHPHY_INSTANCE              gPhyDrvInst[_DRV_ETHPHY_INSTANCES_NUMBER];


// *****************************************************************************
// *****************************************************************************
// Section: Driver Function for derived objects
// *****************************************************************************
// *****************************************************************************
#if (_DRV_ETHPHY_INSTANCES_NUMBER == 1)
DRV_ETHPHY_INSTANCE* _DRV_ETHPHY_HandleToInst(void* handle)
{
    DRV_ETHPHY_INSTANCE* pPhyInst = (DRV_ETHPHY_INSTANCE*)handle;
    return pPhyInst == gPhyDrvInst ? pPhyInst : 0;
}
#else
// multiple instances version
// could be refined more
DRV_ETHPHY_INSTANCE* _DRV_ETHPHY_HandleToInst(void * handle)
{
    DRV_ETHPHY_INSTANCE* pPhyInst = (DRV_ETHPHY_INSTANCE*)handle;
    int phyIx = (int)(pPhyInst - gPhyDrvInst);
    if(phyIx >= 0 && phyIx < sizeof(gPhyDrvInst) / sizeof(*gPhyDrvInst))
    {
        if(pPhyInst == gPhyDrvInst + phyIx)
        {
            return pPhyInst;
        }
    }

    return 0;
}
#endif  // (_DRV_ETHPHY_INSTANCES_NUMBER == 1)

void _DRV_ETHPHY_SetOperDoneResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_RESULT res)
{
    hClientObj->operPhase =  hClientObj->operSubPhase = 0;
    hClientObj->operType = DRV_ETHPHY_CLIENT_OP_TYPE_NONE;
    hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_READY;
    hClientObj->operRes = res;
    if(res < 0)
    {   
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_OP_FAIL) != 0)
        SYS_CONSOLE_PRINT("DRV_PHY operation error: %d\r\n", res);
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_OP_FAIL) != 0)
    }
}

void _DRV_ETHPHY_SetOperStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE opType, DRV_ETHPHY_RESULT res)
{
    hClientObj->operType = opType;
    hClientObj->operPhase = hClientObj->operSubPhase = 0; 
    hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_BUSY; 
    hClientObj->operRes = res;
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
     .DRV_ETHPHY_VendorSMIWriteWaitComplete = DRV_ETHPHY_VendorSMIWriteWaitComplete,
     .DRV_ETHPHY_VendorSMIOperationIsComplete = DRV_ETHPHY_VendorSMIOperationIsComplete,
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
    DRV_ETHPHY_INSTANCE * hSysObj;
    DRV_ETHPHY_INIT *ethphyInit = NULL;

    if ( iModule >= _DRV_ETHPHY_INSTANCES_NUMBER )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    hSysObj = gPhyDrvInst + iModule;
    if( hSysObj->objInUse == true )
    {   // already initalized
        return ( SYS_MODULE_OBJ )hSysObj ;
    }

    /* Assign to the local pointer the init data passed */
    ethphyInit = ( DRV_ETHPHY_INIT * ) init;

    if((hSysObj->pMiimBase = ethphyInit->pMiimObject) == 0)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    hSysObj->miimIndex = ethphyInit->miimIndex;

    hSysObj->objInUse = true;      // Set object to be in use
    hSysObj->status = SYS_STATUS_READY; // Set module state
    hSysObj->iModule  = iModule;  // Store driver instance
    hSysObj->ethphyId = ethphyInit->ethphyId; // Store PLIB ID

    // Assign External PHY Service Functions
    hSysObj->pPhyObj = ethphyInit->pPhyObject;
    hSysObj->pBaseObj = gDrvEthBaseObj;
    
    hSysObj->ethphyTmo = ethphyInit->ethphyTmo;
    
    hSysObj->objClient.clientInUse = 0;
    hSysObj->objClient.detectMask = hSysObj->pPhyObj->bmconDetectMask;
    if(hSysObj->objClient.detectMask == 0)
    {   // use a default value...
       hSysObj->objClient.detectMask = _PHY_BMCON_DETECT_MASK;
    }
    hSysObj->objClient.capabMask = hSysObj->pPhyObj->bmstatCpblMask;

    // hush compiler warning
    _DRV_ETHPHY_AssertCond(true, __func__, __LINE__);
    _PhyDebugCond(true, __func__, __LINE__);

    /* Return the driver handle */
    return ( SYS_MODULE_OBJ )hSysObj ;

}


void DRV_ETHPHY_Reinitialize( SYS_MODULE_OBJ        object ,
                              const SYS_MODULE_INIT * const init )
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst((void*)object);
    if(phyInst != 0)
    {
        DRV_ETHPHY_INIT * ethphyInit = (DRV_ETHPHY_INIT *)init;

        phyInst->status = SYS_STATUS_READY; // Set module state
        phyInst->ethphyId = ethphyInit->ethphyId; // Store PLIB ID

        // Assign External PHY Service Functions
        phyInst->pPhyObj = ethphyInit->pPhyObject;

        phyInst->objClient.clientInUse = 0;
    }
}



void DRV_ETHPHY_Deinitialize( SYS_MODULE_OBJ object )
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst((void*)object);
    if(phyInst != 0)
    {
        /* Set the Device Status */
        phyInst->status  = SYS_STATUS_UNINITIALIZED;

        /* Remove the driver usage */
        phyInst->objInUse  = false;

        phyInst->pPhyObj = 0;
    }

} 


SYS_STATUS DRV_ETHPHY_Status( SYS_MODULE_OBJ object )
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst((void*)object);
    if(phyInst != 0)
    {
        /* Return the status associated with the driver handle */
        return( phyInst->status ) ;
    }

    return SYS_STATUS_ERROR;
} 


void DRV_ETHPHY_Tasks( SYS_MODULE_OBJ object )
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst((void*)object);
    if(phyInst != 0)
    {
        DRV_ETHPHY_CLIENT_OBJ* hClientObj = &phyInst->objClient;

        if(hClientObj->clientInUse != 0)
        {   // active client
            (*_DRV_PHY_ClientOpTbl[hClientObj->operType])(hClientObj);
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
    DRV_ETHPHY_INSTANCE * phyInst;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj;

    /* Validate the driver index */
    /* If there is anything specific to the module & needs to be checked, should
       be handled in this section with an || condition.
       May be something like ioIntent test for Exclusive access */
    if ( iModule >= _DRV_ETHPHY_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    phyInst = gPhyDrvInst + iModule; 
    if(phyInst->objInUse == false)
    {   // not opened
        return DRV_HANDLE_INVALID;
    }

    /* Setup client operations */

    // Allocate the client object and set the flag as in use
    // there's only one client: the MAC
    hClientObj = phyInst->objClient.clientInUse == 0 ?  &phyInst->objClient : 0;

    while(hClientObj)
    {
        DRV_HANDLE miimHandle = phyInst->pMiimBase->DRV_MIIM_Open(phyInst->miimIndex, DRV_IO_INTENT_SHARED);
        if(miimHandle == DRV_HANDLE_INVALID)
        {
            _PhyDebugCond(false, __func__, __LINE__);
            break;
        }

        // set the MIIM driver so it can be used by the PHY setup procedure
        DRV_MIIM_SETUP  miimSetup;

        miimSetup.hostClockFreq = (uint32_t)TCPIP_INTMAC_PERIPHERAL_CLK;


        miimSetup.maxBusFreq = (*phyInst->pPhyObj->smiClockGet)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj);
        miimSetup.setupFlags = DRV_MIIM_SETUP_FLAG_NONE;

        DRV_MIIM_RESULT miimRes = phyInst->pMiimBase->DRV_MIIM_Setup(miimHandle, &miimSetup); 
        if(miimRes < 0)
        {   // error occurred
            phyInst->pMiimBase->DRV_MIIM_Close( miimHandle);
            _PhyDebugCond(false, __func__, __LINE__);
            return DRV_HANDLE_INVALID;
        }

        hClientObj->pMiimBase = phyInst->pMiimBase;
        hClientObj->miimHandle = miimHandle;
        hClientObj->miimOpHandle = 0;

        hClientObj->clientInUse    = true;
        hClientObj->hDriver  = phyInst;
        hClientObj->ethphyId = phyInst->ethphyId;
        hClientObj->operType = DRV_ETHPHY_CLIENT_OP_TYPE_NONE;
        hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
        _DRV_PHY_SetOperPhase(hClientObj, 0, 0);

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
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
            hClientObj->pMiimBase->DRV_MIIM_Close(hClientObj->miimHandle);

            /* Free the Client Instance */
            hClientObj->clientInUse = false ;

            /* Update the Client Status */
            hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_CLOSED;
        }
    }
}

DRV_ETHPHY_RESULT DRV_ETHPHY_Setup( DRV_HANDLE handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
{

    const DRV_ETHPHY_OBJECT* pPhyObj;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
    if(phyInst == 0)
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
    pPhyObj = phyInst->pPhyObj;

    if(pPhyObj == 0 || pPhyObj->miiConfigure == 0 || pPhyObj->mdixConfigure == 0 || pPhyObj->smiClockGet == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if(pSetUp->resetFunction)
    {
        (*pSetUp->resetFunction)(gDrvEthBaseObj, handle);
    }

    phyInst->phyAddress = pSetUp->phyAddress;
    phyInst->openFlags = pSetUp->openFlags;
    phyInst->configFlags = pSetUp->configFlags;
    phyInst->macPauseType = pSetUp->macPauseType;
    
    hClientObj->operParam = (uintptr_t)pSetupFlags;
    _DRV_ETHPHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_SETUP, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}

// DRV_ETHPHY_CLIENT_OP_TYPE_NONE
static void _DRV_ETHPHY_ClientOpNone(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
}

// DRV_ETHPHY_CLIENT_OP_TYPE_SETUP
static void _DRV_ETHPHY_ClientOpSetup(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if((unsigned int)hClientObj->operPhase >= sizeof(_DRV_PHY_SetupPhasesTbl)/sizeof(*_DRV_PHY_SetupPhasesTbl))
    {   // shouldn't happen
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
    else
    {
        (*_DRV_PHY_SetupPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
}


static void _DRV_ETHPHY_SetupPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    DRV_ETHPHY_INSTANCE * hDriver = NULL;
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
            _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_CFG_ERR);
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

    if (configFlags & DRV_ETHPHY_CFG_GMII)
    {
        openFlags |= TCPIP_ETH_OPEN_GMII;
    }
    else if (configFlags & DRV_ETHPHY_CFG_RGMII)
    {
        openFlags |= TCPIP_ETH_OPEN_RGMII;
    }
    else if (configFlags & DRV_ETHPHY_CFG_MII)
    {
        openFlags |= TCPIP_ETH_OPEN_MII;
    }
    else if (configFlags & DRV_ETHPHY_CFG_RMII)
    {
        openFlags |= TCPIP_ETH_OPEN_RMII;
    }
    


    hDriver->openFlags = openFlags;

    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 0);
}

    

static void _DRV_ETHPHY_SetupPhaseDetect(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_VENDOR_DETECT detectF;
    DRV_ETHPHY_INSTANCE* hDriver;
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
            hClientObj->operTStamp = SYS_TMR_TickCountGet() + (((hDriver->ethphyTmo->resetTmo) * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 1);
            break;

        case 1:
            // perform the transfer
            res =  hClientObj->vendorDetect(gDrvEthBaseObj, (DRV_HANDLE)hClientObj);
            if(res == DRV_ETHPHY_RES_OK)
            {   // everything seems to be fine
                // initiate the PHY ID Read
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_READ_ID, 0);
            }
            else if(res < 0)
            {   // failed 
                _DRV_ETHPHY_SetOperDoneResult(hClientObj, res);
            }
            else
            {   // DRV_ETHPHY_RES_PENDING wait some more...
                if(SYS_TMR_TickCountGet() >= hClientObj->operTStamp)
                {   // tmo occurred
                    _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_TMO);
                }
            }
            break;

        default:
            // shouldn't happen
            _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }

}


// default PHY detection procedure
// the DRV_ETHPHY_OBJECT can specify a BMCON detectMask
//      if this mask == 0, then the default procedure will use the BMCON.RESET and BMCON.LOOPBACK bits
//      i.e. the corresponding BMCON_LOOPBACK_MASK | _BMCON_DUPLEX_MASK BMCON mask
// else the DRV_ETHPHY_OBJECT::bmconDetectMask will be used!
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
    uint16_t    detectMask;
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

    _DRV_ETHPHY_Dbg_DetectPhase(detectPhase);

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
            _DRV_ETHPHY_Dbg_DetectReadValue(PHY_REG_BMCON, bmcon.w, _BMCON_RESET_MASK, 0);
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
            bmcon.w = ((DRV_ETHPHY_CLIENT_OBJ*)hClientObj)->detectMask;
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, bmcon.w, phyAddress);
            _DRV_ETHPHY_Dbg_DetectWriteValue(PHY_REG_BMCON, bmcon.w);
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
            detectMask = ((DRV_ETHPHY_CLIENT_OBJ*)hClientObj)->detectMask;
            _DRV_ETHPHY_Dbg_DetectReadValue(PHY_REG_BMCON, bmcon.w, detectMask, detectMask);
            if((bmcon.w & detectMask) != detectMask)
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
            bmcon.w ^= ((DRV_ETHPHY_CLIENT_OBJ*)hClientObj)->detectMask;
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, bmcon.w, phyAddress);
            _DRV_ETHPHY_Dbg_DetectWriteValue(PHY_REG_BMCON, bmcon.w);
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
            detectMask = ((DRV_ETHPHY_CLIENT_OBJ*)hClientObj)->detectMask;
            _DRV_ETHPHY_Dbg_DetectReadValue(PHY_REG_BMCON, bmcon.w, detectMask, 0);
            if((bmcon.w & detectMask) != 0)
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
static void _DRV_ETHPHY_SetupPhaseReadId(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    switch(hClientObj->operSubPhase)
    {
        case 0:
            // read the PHY Identifier 1 (2h) register
            if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_PHYID1))
            {
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_READ_ID, 1);
            }
            break;
        case 1:
            // wait the register read to complete
            if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
            {
                break;
            }     
            hClientObj->phyId_1 = hClientObj->smiData;
            
            // read the PHY Identifier 2 (3h) register
            if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_PHYID2))
            {
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_READ_ID, 2);
            }
            break;

        case 2:
            // wait the register read to complete
            if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
            {
                break;
            }

            hClientObj->phyId_2 = hClientObj->smiData;
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 0);
            break;

        default:
            // shouldn't happen
            _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
            break;
    }

}

// performs the PHY reset
static void _DRV_ETHPHY_SetupPhaseReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t bmcon;
    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
    
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
                hClientObj->operTStamp = SYS_TMR_TickCountGet() + (((phyInst->ethphyTmo->resetTmo) * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
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
                    _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
                }
            }
            else
            {   // success
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 0);
            }
            break;

        default:
            // shouldn't happen
            _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
            break;
    }

}

static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Start(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_Std_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_Ext_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Config_MII(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Config_MDIX(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Write_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_1000B_Ctrl(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Write_1000B_Ctrl(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_AutoNeg_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Start_AutoNeg(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Finish(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

typedef enum
{
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_START = 0,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_STD_STATUS,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_EXT_STATUS,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_CONFIG_MII,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_CONFIG_MDIX,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_WRITE_ANAD,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_1000B_CTRL,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_WRITE_1000B_CTRL,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_AN_STATUS,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_START_AN,
    DRV_ETHPHY_SETUP_NEG_SUBPHASE_FINISH,
} DRV_ETHPHY_SETUP_NEG_PHASE;


static const _DRV_PHY_OperPhaseF _DRV_PHY_SetupPhaseNegotiateSubPhaseTbl[] = 
{
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Start,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_Std_Status,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_Ext_Status,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Config_MII,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Config_MDIX,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Write_ANAD,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_1000B_Ctrl,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Write_1000B_Ctrl,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_AutoNeg_Status,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Start_AutoNeg,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Finish,
};

static void _DRV_ETHPHY_SetupPhaseNegotiate(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(hClientObj->operSubPhase <  sizeof(_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl) / sizeof(*_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl))
    {
        (*_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl[hClientObj->operSubPhase])(hClientObj);
    } 
    else
    {   // shouldn't happen
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

// DRV_ETHPHY_SETUP_NEG_SUBPHASE_START
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Start(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    TCPIP_ETH_OPEN_FLAGS  openFlags;      // flags required at open time
    
    // provide some defaults
    openFlags = hClientObj->hDriver->openFlags;
    if(!(openFlags & (TCPIP_ETH_OPEN_FDUPLEX|TCPIP_ETH_OPEN_HDUPLEX)))
    {
        openFlags |= TCPIP_ETH_OPEN_HDUPLEX;
    }
    if(!(openFlags & (TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_10|TCPIP_ETH_OPEN_1000)))
    {
        openFlags |= TCPIP_ETH_OPEN_10;
    }

    hClientObj->hDriver->openFlags = openFlags;   
    // try to match the openFlags with the PHY capabilities
    // initiate the read of the PHY capabilities
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_STD_STATUS);
    }

}
    
// DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_STD_STATUS
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_Std_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    TCPIP_ETH_OPEN_FLAGS  openFlags;      // flags required at open time
    uint16_t  stdReqs = 0;
    uint16_t  phyBmstatCpbl;
    uint16_t  matchStdCpbl = 0;
    
    // wait the BMCON PHY_REG_BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }
    phyBmstatCpbl = hClientObj->smiData; // BMSTAT value
    phyBmstatCpbl |= hClientObj->capabMask; // use vendor bmstatCpblMask 
    
    // provide some defaults
    openFlags = hClientObj->hDriver->openFlags;
    if(!(openFlags & (TCPIP_ETH_OPEN_FDUPLEX|TCPIP_ETH_OPEN_HDUPLEX)))
    {
        openFlags |= TCPIP_ETH_OPEN_HDUPLEX;
    }
    if(!(openFlags & (TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_10|TCPIP_ETH_OPEN_1000)))
    {
        openFlags |= TCPIP_ETH_OPEN_10;
    }

    if(openFlags & TCPIP_ETH_OPEN_AUTO)
    {   // advertise auto negotiation
        stdReqs = _BMSTAT_AN_ABLE_MASK;
                
        if(openFlags & TCPIP_ETH_OPEN_100)
        {
            if(openFlags & TCPIP_ETH_OPEN_FDUPLEX)
            {
                stdReqs |= _BMSTAT_BASE100TX_FDX_MASK;
            }
            if(openFlags & TCPIP_ETH_OPEN_HDUPLEX)
            {
                stdReqs |= _BMSTAT_BASE100TX_HDX_MASK;
            }
        }

        if(openFlags & TCPIP_ETH_OPEN_10)
        {
            if(openFlags & TCPIP_ETH_OPEN_FDUPLEX)
            {
                stdReqs |= _BMSTAT_BASE10T_FDX_MASK;
            }
            if(openFlags & TCPIP_ETH_OPEN_HDUPLEX)
            {
                stdReqs |= _BMSTAT_BASE10T_HDX_MASK;
            }
        }
    }
    else
    {   // no auto negotiation
        if(openFlags & TCPIP_ETH_OPEN_100)
        {
            stdReqs = (openFlags & TCPIP_ETH_OPEN_FDUPLEX) ? _BMSTAT_BASE100TX_FDX_MASK : _BMSTAT_BASE100TX_HDX_MASK;
        }
        else
        {
            stdReqs = (openFlags & TCPIP_ETH_OPEN_FDUPLEX) ? _BMSTAT_BASE10T_FDX_MASK : _BMSTAT_BASE10T_HDX_MASK;
        }
    }

    // Standard Capabilities of PHY compared with Standard features Requested in configuration 
    matchStdCpbl = ((stdReqs & (_PHY_STD_CPBL_MASK | _BMSTAT_AN_ABLE_MASK))| _BMSTAT_EXTSTAT_MASK | _BMSTAT_EXTEND_ABLE_MASK) & phyBmstatCpbl; 
    
    if(!matchStdCpbl && !(phyBmstatCpbl & _BMSTAT_EXTSTAT_MASK) )
    {   // no match?
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_CPBL_ERR);
        return;
    }

    // we're ok, we can configure the PHY
    hClientObj->hDriver->openFlags = openFlags;
    hClientObj->operReg[0] = matchStdCpbl;
    hClientObj->vendorData = 0;
    
    // Check Phy is extended register capable and extended status capable
    if ((phyBmstatCpbl & _BMSTAT_EXTSTAT_MASK) && (phyBmstatCpbl & _BMSTAT_EXTEND_ABLE_MASK))
    {
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_EXTSTAT))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_EXT_STATUS);
        }
    }
    else
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_CONFIG_MII);
    }

}


// DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_EXT_STATUS
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_Ext_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    TCPIP_ETH_OPEN_FLAGS  openFlags;      // flags required at open time
    static uint16_t phyExtCpbl = 0;
    uint16_t  matchExtCpbl = 0;
    uint16_t  extReqs = 0;
    
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }
    phyExtCpbl = hClientObj->smiData;
    
    // provide some defaults
    openFlags = hClientObj->hDriver->openFlags;   

    if(openFlags & TCPIP_ETH_OPEN_AUTO)
    {
        if(openFlags & TCPIP_ETH_OPEN_1000)
        {
            if(openFlags & TCPIP_ETH_OPEN_FDUPLEX)
            {
                extReqs |= _EXTSTAT_1000BASET_FDX_MASK;
            }
            if(openFlags & TCPIP_ETH_OPEN_HDUPLEX)
            {
                extReqs |= _EXTSTAT_1000BASET_HDX_MASK;
            }
        }     
    }
    else
    {   // no auto negotiation
        if(openFlags & TCPIP_ETH_OPEN_1000)
        {
            extReqs = (openFlags & TCPIP_ETH_OPEN_FDUPLEX) ? _EXTSTAT_1000BASET_FDX_MASK : _EXTSTAT_1000BASET_HDX_MASK;
        }
    }
    
    matchExtCpbl = (extReqs & _PHY_EXT_CPBL_MASK) & phyExtCpbl;
    
    hClientObj->operReg[1] = matchExtCpbl;
    hClientObj->vendorData = 0;
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_CONFIG_MII);  
    
}

// MII/RMII configuration phase
// DRV_ETHPHY_SETUP_NEG_SUBPHASE_CONFIG_MII
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Config_MII(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_INSTANCE *hDriver;
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;

    hDriver = hClientObj->hDriver;
    res = (*hDriver->pPhyObj->miiConfigure)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj, hDriver->configFlags);

    if(res == DRV_ETHPHY_RES_OK)
    {   // success, advance
        // next phase: configure the MDIX
        hClientObj->vendorData = 0;
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_CONFIG_MDIX);
    }
    else if(res != DRV_ETHPHY_RES_PENDING)
    {   // some error occurred
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, res);
    }

}


// MDIX configure
// DRV_ETHPHY_SETUP_NEG_SUBPHASE_CONFIG_MDIX
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Config_MDIX(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_RESULT res;
    DRV_ETHPHY_INSTANCE *hDriver = hClientObj->hDriver;

    res = (*hDriver->pPhyObj->mdixConfigure)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj, hClientObj->hDriver->openFlags);

    if(res < 0)
    {   // some error occurred
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, res);
        return;
    }
    else if(res != DRV_ETHPHY_RES_OK)
    {   // still waiting
        return;
    }

    // success, advance;
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_WRITE_ANAD);
}


// DRV_ETHPHY_SETUP_NEG_SUBPHASE_WRITE_ANAD
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Write_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_INSTANCE *hDriver = hClientObj->hDriver;
    // restore match capabilities
    uint16_t  matchStdCpbl = hClientObj->operReg[0];
    TCPIP_ETH_OPEN_FLAGS  openFlags = hClientObj->hDriver->openFlags;

    if((matchStdCpbl &_BMSTAT_AN_ABLE_MASK) && (openFlags & TCPIP_ETH_OPEN_AUTO))
    {   // ok, we can perform auto negotiation
        uint16_t anadReg = 0;

        anadReg = (((matchStdCpbl >> _BMSTAT_CAPABILITY_POS) << _ANAD_CAPABLITY_POS) & _ANAD_NEGOTIATION_MASK) | _PHY_PROT_802_3;
        
        //Errata: Do not set asymmetric pause for KSZ9031 PHY        
        if(hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_PAUSE)
        {
            anadReg |= _ANAD_PAUSE_MASK;
        }
        if (!((hClientObj->phyId_1 == KSZ9031_PHYID1) && (((hClientObj->phyId_2)&(KSZ9031_PHYID2)) == KSZ9031_PHYID2)))
        {    
            if(hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_ASM_DIR)
            {
               anadReg |= _ANAD_ASM_DIR_MASK;
            } 
        }

        // advertise our capabilities
        if(_DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_ANAD, anadReg))
        {   // check for extended capabilities
            if ((matchStdCpbl & _BMSTAT_EXTSTAT_MASK) && (matchStdCpbl & _BMSTAT_EXTEND_ABLE_MASK))
            {
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_1000B_CTRL);
            }
            else
            {
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_AN_STATUS);
            }
        }
    }
    else
    {   // ok, just don't use negotiation
        uint16_t  ctrlReg;

        ctrlReg = 0;
        
        if((matchStdCpbl & (_BMSTAT_BASE100TX_HDX_MASK | _BMSTAT_BASE100TX_FDX_MASK)) && (openFlags & TCPIP_ETH_OPEN_100))  // set 100Mbps request/capability
        {
            ctrlReg |= _BMCON_SPEED100_MASK;
        }

        if(matchStdCpbl & (_BMSTAT_BASE10T_FDX_MASK | _BMSTAT_BASE100TX_FDX_MASK ))
        {
            ctrlReg |= _BMCON_DUPLEX_MASK;
        }

        if(hClientObj->hDriver->openFlags & TCPIP_ETH_OPEN_PHY_LOOPBACK)
        {
            ctrlReg |= _BMCON_LOOPBACK_MASK;
        }

        if(_DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, ctrlReg))
        {   
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_FINISH);
        }
    }

}

// DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_1000B_CTRL
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_1000B_Ctrl(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_1000BASECON))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_WRITE_1000B_CTRL);
    }    
    
}

// DRV_ETHPHY_SETUP_NEG_SUBPHASE_WRITE_1000B_CTRL
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Write_1000B_Ctrl(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    uint16_t ext1000baseCtrl = 0, matchExtStat = 0;
    
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }
    ext1000baseCtrl = hClientObj->smiData;
    matchExtStat = hClientObj->operReg[1];
    
    if(matchExtStat & _EXTSTAT_1000BASET_FDX_MASK)
    {
        ext1000baseCtrl |= _1000BASE_TCTRL_TFDX_MASK;
    }
    else
    {
        ext1000baseCtrl &= ~_1000BASE_TCTRL_TFDX_MASK;
    }
    if(matchExtStat & _EXTSTAT_1000BASET_HDX_MASK)
    {
        ext1000baseCtrl |= _1000BASE_TCTRL_THDX_MASK;
    }
    else
    {
        ext1000baseCtrl &= ~_1000BASE_TCTRL_THDX_MASK;
    }
    
    if(_DRV_PHY_SMIWriteStart( hClientObj, PHY_REG_1000BASECON, ext1000baseCtrl))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_AN_STATUS);
    }
    
}

// advertise, negotiation restart phase
// DRV_ETHPHY_SETUP_NEG_SUBPHASE_READ_AN_STATUS
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Read_AutoNeg_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
    {   // read capabilities initiate
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_START_AN);
    }
}
    
// auto-negotiation perform
// DRV_ETHPHY_SETUP_NEG_SUBPHASE_START_AN
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Start_AutoNeg(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the PHY_REG_BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    __BMSTATbits_t  phyStdCpbl;

    phyStdCpbl.w = hClientObj->smiData;

    if(phyStdCpbl.AN_ABLE)
    {   // ok, we can perform auto negotiation
        // restart negotiation and we'll have to wait
        if(_DRV_PHY_SMIWriteStart( hClientObj, PHY_REG_BMCON, _BMCON_AN_ENABLE_MASK | _BMCON_AN_RESTART_MASK))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, DRV_ETHPHY_SETUP_NEG_SUBPHASE_FINISH);
        }
    }
    else
    {   // no negotiation ability!
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_UNABLE);
    }
}

// final update stage
// DRV_ETHPHY_SETUP_NEG_SUBPHASE_FINISH
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase_Finish(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    TCPIP_ETH_OPEN_FLAGS openFlags = TCPIP_ETH_OPEN_DEFAULT;
    uint16_t  matchStdCpbl = hClientObj->operReg[0];
    uint16_t  matchExtCpbl = hClientObj->operReg[1];
    
    // now update the open flags
    // the upper layer needs to know the PHY set-up to further set-up the MAC.
    openFlags = hClientObj->hDriver->openFlags;


    if(!(matchStdCpbl &_BMSTAT_AN_ABLE_MASK))
    {       
        openFlags &= ~TCPIP_ETH_OPEN_AUTO;
    }
    
    // set 1000Mbps request/capability
    if(!(matchExtCpbl & (_EXTSTAT_1000BASET_FDX_MASK | _EXTSTAT_1000BASET_HDX_MASK))) 
    {
        openFlags &= ~TCPIP_ETH_OPEN_1000;
    }
    
    // set 100Mbps request/capability
    if(!(matchStdCpbl & (_BMSTAT_BASE100TX_HDX_MASK | _BMSTAT_BASE100TX_FDX_MASK)))  
    {
        openFlags &= ~TCPIP_ETH_OPEN_100;
    }
    
    // set 10Mbps request/capability
    if(!(matchStdCpbl & (_BMSTAT_BASE10T_HDX_MASK | _BMSTAT_BASE10T_FDX_MASK)))  
    {
        openFlags &= ~TCPIP_ETH_OPEN_10;
    }
    
    if(!((matchStdCpbl & (_BMSTAT_BASE10T_FDX_MASK | _BMSTAT_BASE100TX_FDX_MASK)) || (matchExtCpbl & _BASE1000T_FDX_MASK )))
    {
        openFlags &= ~TCPIP_ETH_OPEN_FDUPLEX;
    }
    
    if(!((matchStdCpbl & (_BMSTAT_BASE10T_HDX_MASK | _BMSTAT_BASE100TX_HDX_MASK)) || (matchExtCpbl & _BASE1000T_HDX_MASK )))
    {
        openFlags &= ~TCPIP_ETH_OPEN_HDUPLEX;
    }
   
    // store the openFlags!
    // upper layer needs to know the PHY set-up to further set-up the MAC.
    hClientObj->hDriver->openFlags = openFlags;

    // we're done
    // deposit result
    TCPIP_ETH_OPEN_FLAGS* pSetUpFlags = (TCPIP_ETH_OPEN_FLAGS*)hClientObj->operParam;
    *pSetUpFlags = openFlags;
     _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);

} 

DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_ClientStatus( DRV_HANDLE handle )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
            /* Return the client status associated with the handle passed */
            return( hClientObj->status );
        }
    }

    return DRV_ETHPHY_CLIENT_STATUS_ERROR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationResult( DRV_HANDLE handle)  
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
            /* Return the client status associated with the handle passed */
            return( hClientObj->operRes );
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationAbort( DRV_HANDLE handle)  
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
            _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_ABORTED);
            return DRV_ETHPHY_RES_OK;
        }
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
    if( hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
            if(pPhyAddress)
            {
                *pPhyAddress = hClientObj->hDriver->phyAddress;
            }
            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
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
    if(hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {

            if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            _DRV_ETHPHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESTART, DRV_ETHPHY_RES_PENDING);
            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESTART
static void _DRV_ETHPHY_ClientOpNegRestart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegRestartPhasesTbl)/sizeof(*_DRV_PHY_NegRestartPhasesTbl))
    {
        (*_DRV_PHY_NegRestartPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
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
    __BMSTATbits_t  phyStdCpbl;

    // wait the BMSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyStdCpbl.w = hClientObj->smiData;
    if(phyStdCpbl.AN_ABLE == 0)
    {   // no negotiation ability!
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_UNABLE);
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
    _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}

DRV_ETHPHY_RESULT DRV_ETHPHY_HWConfigFlagsGet( DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags )
{
    //#todo# remove direct MAC register access from this function 
    DRV_ETHPHY_CONFIG_FLAGS hwFlags = DRV_ETHPHY_CFG_RMII;
    DRV_ETHPHY_RESULT ethRes = DRV_ETHPHY_RES_CFG_ERR;

    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    if(hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
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
#elif defined (TCPIP_IF_PIC32CXBZ6_ETH)
            hwFlags = ((ETH_REGS->ETH_UR & ETH_UR_Msk)== DRV_ETH_RMII_MODE) ?  DRV_ETHPHY_CFG_RMII : DRV_ETHPHY_CFG_MII;
            ethRes = DRV_ETHPHY_RES_OK;            
#elif defined (TCPIP_IF_PIC32CZCA8_CA9_ETH)
            if (ETH_REGS->ETH_CTRLB & ETH_CTRLB_GMIIEN_Msk)
            {
                if(ETH_REGS->ETH_NCFGR & ETH_NCFGR_GIGE_Msk)
                {
                    hwFlags = DRV_ETHPHY_CFG_GMII;
                }
                else
                {
                    hwFlags = DRV_ETHPHY_CFG_MII;
                }
            }
            else
            {
                hwFlags = DRV_ETHPHY_CFG_RMII;
            }
           
            ethRes = DRV_ETHPHY_RES_OK;
#elif defined(DRV_SAMA7G) && (defined(DRV_GMAC0) || defined(DRV_GMAC1))
            gmac_registers_t *  pGmacRegs = (gmac_registers_t *) hClientObj->ethphyId;
            if(pGmacRegs->GMAC_NCR & GMAC_NCR_MIIONRGMII_Msk)
            {
                if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == 0) //GMAC_UR_MIM_MII
                {
                    hwFlags = DRV_ETHPHY_CFG_MII;
                    ethRes = DRV_ETHPHY_RES_OK;
                }
            }
            else
            {
                if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == GMAC_UR_MIM_RGMII)
                {
                    hwFlags = DRV_ETHPHY_CFG_RGMII;
                    ethRes = DRV_ETHPHY_RES_OK;
                }
                else if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == GMAC_UR_MIM_RMII)
                {
                    hwFlags = DRV_ETHPHY_CFG_RMII;
                    ethRes = DRV_ETHPHY_RES_OK;
                }
            }  
#elif defined(TCPIP_IF_GMAC_SAM9X7) || defined(DRV_SAMA7D6)
            gmac_registers_t *  pGmacRegs = (gmac_registers_t *) hClientObj->ethphyId;
            if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == GMAC_UR_MIM_RGMII)
            {
                hwFlags = DRV_ETHPHY_CFG_RGMII;
                ethRes = DRV_ETHPHY_RES_OK;
            }
            else if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == GMAC_UR_MIM_RMII)
            {
                hwFlags = DRV_ETHPHY_CFG_RMII;
                ethRes = DRV_ETHPHY_RES_OK;
            }            
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
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationIsComplete( DRV_HANDLE hClient, DRV_ETHPHY_INTERFACE_INDEX portIndex, bool waitComplete )
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) hClient;

    /* Check for the Client validity */
    if(hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
            if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            hClientObj->operParam = waitComplete;
            _DRV_ETHPHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_COMPLETE, DRV_ETHPHY_RES_PENDING);
            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// DRV_ETHPHY_CLIENT_OP_TYPE_NEG_COMPLETE
static void _DRV_ETHPHY_ClientOpNegComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegCompletePhasesTbl)/sizeof(*_DRV_PHY_NegCompletePhasesTbl))
    {
        (*_DRV_PHY_NegCompletePhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
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
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_INACTIVE);
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
    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
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
    hClientObj->operTStamp = SYS_TMR_TickCountGet() + (((phyInst->ethphyTmo->aNegInitTmo) * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_AN_RESTART, 0);
    }
}

static void _DRV_ETHPHY_NegCompletePhaseAN_Restart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;
    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
    
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
        hClientObj->operTStamp = SYS_TMR_TickCountGet() + (((phyInst->ethphyTmo->aNegDoneTmo) * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_AN_COMPLETE, 0);
        }
        return;
    }

    if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
    {   // wait some more; re-read
        (void)_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
    }
    else
    {   // timeout
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED);
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
            (void)_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
        }
        else
        {   // timeout
            _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
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

    _DRV_ETHPHY_SetOperDoneResult(hClientObj, res);
}



DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
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
            _DRV_ETHPHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESULT, DRV_ETHPHY_RES_PENDING);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


// DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESULT
static void _DRV_ETHPHY_ClientOpNegResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegResultPhasesTbl)/sizeof(*_DRV_PHY_NegResultPhasesTbl))
    {
        (*_DRV_PHY_NegResultPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
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
    else if(phyStat.EXTENDED_STAT == 1)
    {
        // we're up and running
        // read the control reg: Extended Status
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_EXTSTAT))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_EXTSTAT, 0);
        }
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

static void _DRV_ETHPHY_NegResultPhase_EXTSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the EXTSTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    if ((hClientObj->smiData) & (_EXTSTAT_BASE1000X_FDX_MASK | _EXTSTAT_BASE1000X_HDX_MASK | _EXTSTAT_BASE1000T_FDX_MASK | _EXTSTAT_BASE1000T_HDX_MASK))
    {
        // read the control reg: 1000BASET-Control 
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_1000BASECON))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_1000BASECTRL, 0);
        }
    }
    else
    {
        // read the control reg: ANEXP
        if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_ANEXP))
        {
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_ANEXP, 0);
        }
    }
    
}    
static void _DRV_ETHPHY_NegResultPhase_1000BASECTRL(DRV_ETHPHY_CLIENT_OBJ * hClientObj)    
{    
    // wait the 1000BASET-Control read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save the 1000BASET-Control for later
    hClientObj->operReg[2] = hClientObj->smiData;
    // read the control reg: 1000BASESTAT
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_1000BASESTAT))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_1000BASESTAT, 0);
    }

}

static void _DRV_ETHPHY_NegResultPhase_1000BASESTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the 1000BASESTAT read to complete
    if(!_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save the 1000BASESTAT for later
    hClientObj->operReg[3] = hClientObj->smiData;

    // we're up and running
    // read the control reg: ANEXP
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_ANEXP))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_ANEXP, 0);
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
    uint16_t gigbCtrl = 0, gigbStat = 0;
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
    
    gigbCtrl = hClientObj->operReg[2]; 
    gigbStat = hClientObj->operReg[3]; 
    
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
    if((gigbStat & _1000BSTAT_1000T_FDX_MASK) && (gigbCtrl & _1000BASE_TCTRL_TFDX_MASK))
    {
        oFlags = (TCPIP_ETH_OPEN_1000|TCPIP_ETH_OPEN_FDUPLEX);
    }
    else if((gigbStat & _1000BSTAT_1000T_HDX_MASK) && (gigbCtrl & _1000BASE_TCTRL_THDX_MASK))
    {
        oFlags = (TCPIP_ETH_OPEN_1000|TCPIP_ETH_OPEN_HDUPLEX);
    }
    else if(anadReg.w & _ANAD_BASE100TX_FDX_MASK)
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

    _DRV_ETHPHY_SetOperDoneResult(hClientObj, res);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
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
            _DRV_ETHPHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_LINK_STAT, DRV_ETHPHY_RES_PENDING);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}
 
// DRV_ETHPHY_CLIENT_OP_TYPE_LINK_STAT
static void _DRV_ETHPHY_ClientOpLinkStat(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_LinkStatPhasesTbl)/sizeof(*_DRV_PHY_LinkStatPhasesTbl))
    {
        (*_DRV_PHY_LinkStatPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

// DRV_ETHPHY_LINK_STAT_PHASE_IDLE
static void _DRV_ETHPHY_LinkStatPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the link status: PHY_REG_BMSTAT
    if(_DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT))
    {
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_LINK_STAT_PHASE_READ, 0);
    }
}

// read from PHY_REG_BMSTAT
// DRV_ETHPHY_LINK_STAT_PHASE_READ
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
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
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
// DRV_ETHPHY_LINK_STAT_PHASE_REFRESH
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

    _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_Reset( DRV_HANDLE handle, bool waitComplete )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {

            if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            hClientObj->operParam = waitComplete;
            _DRV_ETHPHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_RESET, DRV_ETHPHY_RES_PENDING);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


// DRV_ETHPHY_CLIENT_OP_TYPE_RESET
static void _DRV_ETHPHY_ClientOpReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_ResetPhasesTbl)/sizeof(*_DRV_PHY_ResetPhasesTbl))
    {
        (*_DRV_PHY_ResetPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
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
    DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
    if( hClientObj->operParam == 0)
    {   // no need to wait, we're done
        _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
    }
    else
    {   // read the BMCON
        hClientObj->operTStamp = SYS_TMR_TickCountGet() + (((phyInst->ethphyTmo->resetTmo) * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
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
                    _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
                }
            }
            else
            {   // reset cleared itself; success
                _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
            }
            break;

        default:
            // shouldn't happen
            _DRV_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
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

    if(hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
            if(pVendorData)
            {
                *pVendorData = hClientObj->vendorData;
            }

            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataSet( DRV_HANDLE handle, uint32_t vendorData )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj != 0)
    {
        DRV_ETHPHY_INSTANCE * phyInst = _DRV_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != 0)
        {
            hClientObj->vendorData = vendorData;
            return DRV_ETHPHY_RES_OK;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}



DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadStart( DRV_HANDLE handle, uint16_t rIx,  int phyAddress )
{
    DRV_ETHPHY_SMI_TXFER_RES opRes;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0 || _DRV_ETHPHY_HandleToInst(hClientObj->hDriver) == 0)
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

    if(hClientObj == 0 || _DRV_ETHPHY_HandleToInst(hClientObj->hDriver) == 0)
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

static DRV_ETHPHY_RESULT DRV_ETHPHY_DoSMIWriteStart( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress, bool waitComplete )
{
    DRV_ETHPHY_RESULT phyRes;
    DRV_ETHPHY_SMI_TXFER_RES opRes;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0 || _DRV_ETHPHY_HandleToInst(hClientObj->hDriver) == 0)
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

    opRes = _DRV_PHY_SMIWriteStartEx(hClientObj, rIx, wData, phyAddress, waitComplete);

    if(opRes < 0)
    {   // error
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {   // retry
        phyRes = DRV_ETHPHY_RES_PENDING;
    }
    else
    {   // went through: smiTxferStatus == DRV_ETHPHY_SMI_TXFER_RES_SCHEDULED/DRV_ETHPHY_SMI_TXFER_RES_DONE
        phyRes = DRV_ETHPHY_RES_OK;
    }

    return phyRes;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIWriteStart( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress )
{
    return DRV_ETHPHY_DoSMIWriteStart(handle, rIx,  wData, phyAddress, false);
}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIWriteWaitComplete( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress )
{
    return DRV_ETHPHY_DoSMIWriteStart(handle, rIx,  wData, phyAddress, true);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIOperationIsComplete(DRV_HANDLE handle)
{  
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    
    DRV_ETHPHY_SMI_TXFER_RES opRes = _DRV_PHY_SMITransferWaitComplete(hClientObj);
    if(opRes < 0)
    {   // error
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_WAIT)
    {   // wait some more
        return DRV_ETHPHY_RES_PENDING;
    }
    return DRV_ETHPHY_RES_OK;
}
/*******************************************************************************
End of File
*/
