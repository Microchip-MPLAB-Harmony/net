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
Copyright (C) 2013-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
#define PHY_PROT_802_3         0x01    // IEEE 802.3 capability
// all comm capabilities our MAC supports
#define PHY_STD_CPBL_MASK      ((uint16_t)BMSTAT_BASE10T_HDX_MASK | (uint16_t)BMSTAT_BASE10T_FDX_MASK |      \
                                (uint16_t)BMSTAT_BASE100TX_HDX_MASK |(uint16_t)BMSTAT_BASE100TX_FDX_MASK |    \
                                (uint16_t)BMSTAT_EXTSTAT_MASK)
                                
#define PHY_EXT_CPBL_MASK      ((uint16_t)EXTSTAT_1000BASEX_FDX_MASK | (uint16_t)EXTSTAT_1000BASEX_HDX_MASK |\
                                (uint16_t)EXTSTAT_1000BASET_FDX_MASK | (uint16_t)EXTSTAT_1000BASET_HDX_MASK)


#define PHY_BMCON_DETECT_MASK  ((uint16_t)BMCON_LOOPBACK_MASK | (uint16_t)BMCON_DUPLEX_MASK) 

// local prototypes
// debug
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC) != 0)
static volatile int phyStayAssertLoop = 0;
void F_ETHPHY_AssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PHY Assert: %s, in line: %d, \r\n", message, lineNo);
        while(phyStayAssertLoop != 0)
        {
            ;
        }
            
    }
}
// a debug condition, not really assertion
static volatile int phyStayCondLoop = 0;
static void M_PhyDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PHY Cond: %s, in line: %d, \r\n", message, lineNo);
        while(phyStayCondLoop != 0)
        {
            ;
        }
    }
}

#else
#define M_PhyDebugCond(cond, message, lineNo)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC)

#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_DETECT_PHASE) != 0)
void F_ETHPHY_Dbg_DetectPhase(uint16_t detectPhase)
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
void F_ETHPHY_Dbg_DetectWriteValue(int rIx, uint16_t rVal)
{
    SYS_CONSOLE_PRINT("PHY detect write - reg: %d, val: 0x%04x\r\n", rIx, rVal);
}

void F_ETHPHY_Dbg_DetectReadValue(int rIx, uint16_t rVal, uint16_t valMask, uint16_t chkMask)
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


typedef void (*DRV_PHY_OperPhaseF_T)(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void F_ETHPHY_ClientOpNone(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_ClientOpSetup(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_ClientOpLinkStat(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_ClientOpNegComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_ClientOpNegResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_ClientOpNegRestart(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_ClientOpReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const DRV_PHY_OperPhaseF_T DRV_PHY_ClientOpTbl[] =
{
    &F_ETHPHY_ClientOpNone,           // PHY_CLIENT_OP_TYPE_NONE 
    &F_ETHPHY_ClientOpSetup,          // PHY_CLIENT_OP_TYPE_SETUP
    &F_ETHPHY_ClientOpLinkStat,       // PHY_CLIENT_OP_TYPE_LINK_STAT
    &F_ETHPHY_ClientOpNegComplete,    // PHY_CLIENT_OP_TYPE_NEG_COMPLETE
    &F_ETHPHY_ClientOpNegResult,      // PHY_CLIENT_OP_TYPE_NEG_RESULT
    &F_ETHPHY_ClientOpNegRestart,     // PHY_CLIENT_OP_TYPE_NEG_RESTART
    &F_ETHPHY_ClientOpReset,          // PHY_CLIENT_OP_TYPE_RESET
};


static void F_DRV_ETHPHY_SetupPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_DRV_ETHPHY_SetupPhaseDetect(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_DRV_ETHPHY_SetupPhaseReadId(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_DRV_ETHPHY_SetupPhaseReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_DRV_ETHPHY_SetupPhaseNegotiate(DRV_ETHPHY_CLIENT_OBJ * hClientObj);



static const DRV_PHY_OperPhaseF_T DRV_PHY_SetupPhasesTbl[] = 
{
    &F_DRV_ETHPHY_SetupPhaseIdle,
    &F_DRV_ETHPHY_SetupPhaseDetect,
    &F_DRV_ETHPHY_SetupPhaseReadId,
    &F_DRV_ETHPHY_SetupPhaseReset,
    &F_DRV_ETHPHY_SetupPhaseNegotiate,
};


static void F_ETHPHY_LinkStatPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_LinkStatPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_LinkStatPhaseRefresh(DRV_ETHPHY_CLIENT_OBJ * hClientObj);



static const DRV_PHY_OperPhaseF_T DRV_PHY_LinkStatPhasesTbl[] = 
{
    &F_ETHPHY_LinkStatPhaseIdle,      // DRV_ETHPHY_LINK_STAT_PHASE_IDLE
    &F_ETHPHY_LinkStatPhaseRead,      // DRV_ETHPHY_LINK_STAT_PHASE_READ
    &F_ETHPHY_LinkStatPhaseRefresh,   // DRV_ETHPHY_LINK_STAT_PHASE_REFRESH
};


static void F_ETHPHY_NegCompletePhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegCompletePhaseRead1(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegCompletePhaseRead2(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegCompletePhaseResultNoWait(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegCompletePhaseAN_Restart(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegCompletePhaseAN_Complete(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void F_DRV_PHY_NegCompleteSetOperResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t phyBmConReg, uint16_t phyStatReg);

static const DRV_PHY_OperPhaseF_T DRV_PHY_NegCompletePhasesTbl[] = 
{
    &F_ETHPHY_NegCompletePhaseIdle,
    &F_ETHPHY_NegCompletePhaseRead1,
    &F_ETHPHY_NegCompletePhaseRead2,
    &F_ETHPHY_NegCompletePhaseResultNoWait,
    &F_ETHPHY_NegCompletePhaseAN_Restart,
    &F_ETHPHY_NegCompletePhaseAN_Complete,
};


static void F_ETHPHY_NegResultPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegResultPhase_BMSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegResultPhase_EXTSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegResultPhase_CTRL1000BASE(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegResultPhase_STAT1000BASE(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegResultPhase_ANEXP(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegResultPhase_ANLPAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegResultPhase_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void F_DRV_PHY_NegResultSetOperResult(DRV_ETHPHY_CLIENT_OBJ* hClientObj, DRV_ETHPHY_LINK_STATUS linkStatus,
                                    TCPIP_ETH_OPEN_FLAGS openFlags, TCPIP_ETH_PAUSE_TYPE pauseType, DRV_ETHPHY_RESULT res);

static const DRV_PHY_OperPhaseF_T DRV_PHY_NegResultPhasesTbl[] = 
{
    &F_ETHPHY_NegResultPhaseIdle,
    &F_ETHPHY_NegResultPhase_BMSTAT,
    &F_ETHPHY_NegResultPhase_EXTSTAT,
    &F_ETHPHY_NegResultPhase_CTRL1000BASE,
    &F_ETHPHY_NegResultPhase_STAT1000BASE,
    &F_ETHPHY_NegResultPhase_ANEXP,
    &F_ETHPHY_NegResultPhase_ANLPAD,
    &F_ETHPHY_NegResultPhase_ANAD,
};

static void F_ETHPHY_NegRestartPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegRestartPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_ETHPHY_NegRestartPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const DRV_PHY_OperPhaseF_T DRV_PHY_NegRestartPhasesTbl[] = 
{
    &F_ETHPHY_NegRestartPhaseIdle,
    &F_ETHPHY_NegRestartPhaseRead,
    &F_ETHPHY_NegRestartPhaseWrite,
};

static void F_DRV_ETHPHY_ResetPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_DRV_ETHPHY_ResetPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_DRV_ETHPHY_ResetPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const DRV_PHY_OperPhaseF_T DRV_PHY_ResetPhasesTbl[] = 
{
    &F_DRV_ETHPHY_ResetPhaseIdle,
    &F_DRV_ETHPHY_ResetPhaseWrite,
    &F_DRV_ETHPHY_ResetPhaseRead,
};

// conversion functions/helpers
static __inline__ DRV_ETHPHY_INIT* __attribute__((always_inline)) CF_ModInit2PhyInit(SYS_MODULE_INIT const * const init)
{
    union
    {
        SYS_MODULE_INIT const * pModinit;
        DRV_ETHPHY_INIT *        pPhyinit;
    }U_PHY_INIT;

    U_PHY_INIT.pModinit = init;
    return U_PHY_INIT.pPhyinit;
}

static __inline__ void* __attribute__((always_inline)) CF_SysObj2pVoid(SYS_MODULE_OBJ obj)
{
    union
    {
        SYS_MODULE_OBJ obj;
        void *        pVoid;
    }U_SYSOBJ_PVOID;

    U_SYSOBJ_PVOID.obj = obj;
    return U_SYSOBJ_PVOID.pVoid;
}
// local functions
//

static DRV_ETHPHY_RESULT F_ETHPHY_DefaultDetect( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE hClientObj);

static DRV_ETHPHY_LINK_STATUS F_Phy2LinkStat(BMSTATbits_t phyStat)
{
    DRV_ETHPHY_LINK_STATUS  linkStat;

    linkStat = (phyStat.LINK_STAT != 0U)?DRV_ETHPHY_LINK_ST_UP:DRV_ETHPHY_LINK_ST_DOWN; 
    if(phyStat.REM_FAULT != 0U)
    {
        uint32_t tempLinkStat;
        tempLinkStat = (uint32_t)linkStat| (uint32_t)DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
        linkStat = (DRV_ETHPHY_LINK_STATUS)tempLinkStat;
    }

    return linkStat;

}

// starts a read/write transfer
// returns:
//      - < 0 an error occurred;  smiTxferStatus unchanged (should be DRV_ETHPHY_SMI_TXFER_OP_NONE)
//      - DRV_ETHPHY_SMI_TXFER_RES_BUSY: transaction needs to be retried; smiTxferStatus unchanged (should be DRV_ETHPHY_SMI_TXFER_OP_NONE)
//      - DRV_ETHPHY_SMI_TXFER_RES_SCHEDULED: transaction started (read/write complete); smiTxferStatus == DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE
//      - DRV_ETHPHY_SMI_TXFER_RES_DONE: transaction done (simple write); smiTxferStatus == DRV_ETHPHY_SMI_TXFER_OP_NONE
static DRV_ETHPHY_SMI_TXFER_RES F_DRV_PHY_SMITransferStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_MIIM_OPERATION_HANDLE miimOpHandle;
    DRV_MIIM_RESULT miimRes;
    DRV_ETHPHY_SMI_TXFER_OP_STATUS newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
    DRV_ETHPHY_SMI_TXFER_RES txferRes;
    DRV_MIIM_OPERATION_FLAGS opFlags;
    DRV_ETHPHY_SMI_XFER_TYPE txType = (DRV_ETHPHY_SMI_XFER_TYPE)hClientObj->smiTxferType;

    if(hClientObj->miimOpHandle != NULL)
    {   // no operations queued, need to wait the end of the previous one
        return DRV_ETHPHY_SMI_TXFER_RES_BUSY;
    }

    if(txType == DRV_ETHPHY_SMI_XFER_TYPE_READ)
    {   // start read
        miimOpHandle = hClientObj->pMiimBase->miim_Read(hClientObj->miimHandle, hClientObj->smiRIx, hClientObj->smiPhyAddress, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
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

        miimOpHandle = hClientObj->pMiimBase->miim_Write(hClientObj->miimHandle, hClientObj->smiRIx, hClientObj->smiPhyAddress, hClientObj->smiData, opFlags, &miimRes);
    }
    else
    {   // no other/scan operations allowed that take complete control of DRV_MIIM!
        F_ETHPHY_AssertCond(false, __func__, __LINE__);
        return DRV_ETHPHY_SMI_TXFER_RES_ILLEGAL;
    }

    if(miimOpHandle == NULL)
    {   // MIIM operation failed or needs to be retried
        if(miimRes == DRV_MIIM_RES_BUSY)
        {
            return DRV_ETHPHY_SMI_TXFER_RES_BUSY;   // retry
        }
        else 
        {   // some real error
            M_PhyDebugCond(false, __func__, __LINE__);
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

    hClientObj->smiTxferStatus = (uint8_t)newTxferStatus;
    return txferRes;

}

// waits for a read/write transfer to complete
// returns:
//      - < 0 an error occurred
//      - DRV_ETHPHY_SMI_TXFER_RES_WAIT: not yet complete, call again
//      - DRV_ETHPHY_SMI_TXFER_RES_DONE: transaction completed succesfully
// Note: this should be called only when (hClientObj->smiTxferStatus == DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE
//      i.e. when the SMI F_DRV_PHY_SMITransferStart operation has been called with:
//      DRV_ETHPHY_SMI_XFER_TYPE_READ or DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE
//      For a simple DRV_ETHPHY_SMI_XFER_TYPE_WRITE operation, the result is discarded, so no wait is needed!
static DRV_ETHPHY_SMI_TXFER_RES F_DRV_PHY_SMITransferWaitComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_SMI_TXFER_RES txferRes;
    DRV_MIIM_RESULT miimRes;
    uint32_t  opData;

    if(hClientObj->smiTxferStatus != (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE)
    {   // nothing to wait for; but some functions may call this if a write operation was failed by MIIM and needs retry
        M_PhyDebugCond(false, __func__, __LINE__);
        return DRV_ETHPHY_SMI_TXFER_RES_DONE;
    } 

    if(hClientObj->miimHandle == (DRV_HANDLE)0)
    {   // shouldn't happen
        F_ETHPHY_AssertCond(false, __func__, __LINE__);
        return DRV_ETHPHY_SMI_TXFER_RES_ILLEGAL;
    } 

    miimRes = hClientObj->pMiimBase->miim_OperationResult(hClientObj->miimHandle, hClientObj->miimOpHandle, &opData);
    if(miimRes == DRV_MIIM_RES_PENDING)
    {   // wait op to complete
        return DRV_ETHPHY_SMI_TXFER_RES_WAIT;
    }

    if(miimRes < (DRV_MIIM_RESULT)0)
    {   // some error has occurred
        M_PhyDebugCond(false, __func__, __LINE__);
        txferRes = DRV_ETHPHY_SMI_TXFER_RES_MIIM_ERROR; 
    }
    else
    {
        txferRes = DRV_ETHPHY_SMI_TXFER_RES_DONE; 
        if(hClientObj->smiTxferType == (uint8_t)DRV_ETHPHY_SMI_XFER_TYPE_READ)
        {
            hClientObj->smiData = (uint16_t)opData; // set the read result
        }
    }

    hClientObj->miimOpHandle = NULL;  // operation done
    hClientObj->smiTxferStatus = (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_NONE;
    return txferRes;
}

// waits for a SMI transfer to complete
// returns true if it is done and can move on
// false if it failed or need to wait
static bool F_DRV_PHY_SMITransfer_Wait(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_SMI_TXFER_RES opRes = F_DRV_PHY_SMITransferWaitComplete(hClientObj);

    if(opRes < (DRV_ETHPHY_SMI_TXFER_RES)0)
    {   // error
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
        return false;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_WAIT)
    {   // retry
        return false;
    }
    else
    {
        //Do Nothing
    }

    return true;
}

// starts an SMI read transfer
// returns true if went through
// false if need to wait or there was an error
static bool F_DRV_PHY_SMIReadStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx)
{
    hClientObj->smiTxferStatus = (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = (uint8_t)DRV_ETHPHY_SMI_XFER_TYPE_READ;
    hClientObj->smiPhyAddress = hClientObj->hDriver->phyAddress;

    DRV_ETHPHY_SMI_TXFER_RES opRes = F_DRV_PHY_SMITransferStart(hClientObj);

    if(opRes < (DRV_ETHPHY_SMI_TXFER_RES)0)
    {   // error
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
        return false;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {   // retry
        return false;
    }
    else
    {
        //Do Nothing
    }

    // went through
    return true;
}

static DRV_ETHPHY_SMI_TXFER_RES F_DRV_PHY_SMIReadStartEx(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, int phyAdd)
{
    hClientObj->smiTxferStatus = (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = (uint8_t)DRV_ETHPHY_SMI_XFER_TYPE_READ;
    hClientObj->smiPhyAddress = (uint16_t)phyAdd;

    return F_DRV_PHY_SMITransferStart(hClientObj);
}

// starts an SMI write transfer
// returns true if went through
// false if need to retry or there was an error
static bool F_DRV_PHY_SMIWriteStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, uint16_t wData)
{
    hClientObj->smiTxferStatus = (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = (uint8_t)DRV_ETHPHY_SMI_XFER_TYPE_WRITE;
    hClientObj->smiData =  wData;
    hClientObj->smiPhyAddress = hClientObj->hDriver->phyAddress;

    DRV_ETHPHY_SMI_TXFER_RES opRes = F_DRV_PHY_SMITransferStart(hClientObj);

    if(opRes < (DRV_ETHPHY_SMI_TXFER_RES)0)
    {   // error
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
        return false;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {   // retry
        return false;
    }
    else
    {
        //Do Nothing
    }

    // went through
    return true;
}

static DRV_ETHPHY_SMI_TXFER_RES F_DRV_PHY_SMIWriteStartEx(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, uint16_t wData, int phyAdd, bool waitComplete)
{
    hClientObj->smiTxferStatus = (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = waitComplete ? (uint8_t)DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE : (uint8_t)DRV_ETHPHY_SMI_XFER_TYPE_WRITE;
    hClientObj->smiData =  wData;
    hClientObj->smiPhyAddress = (uint16_t)phyAdd;

   return  F_DRV_PHY_SMITransferStart(hClientObj);
}

static void F_DRV_PHY_SetOperPhase(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t operPhase, uint16_t operSubPhase)
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
    #define M_DRV_ETHPHY_INSTANCES_NUMBER    1
#else
    #define M_DRV_ETHPHY_INSTANCES_NUMBER    TCPIP_STACK_INTMAC_COUNT
#endif

static DRV_ETHPHY_INSTANCE              gPhyDrvInst[M_DRV_ETHPHY_INSTANCES_NUMBER];


// *****************************************************************************
// *****************************************************************************
// Section: Driver Function for derived objects
// *****************************************************************************
// *****************************************************************************
#if (M_DRV_ETHPHY_INSTANCES_NUMBER == 1)
DRV_ETHPHY_INSTANCE* F_ETHPHY_HandleToInst(void* handle)
{
    DRV_ETHPHY_INSTANCE* pPhyInst = (DRV_ETHPHY_INSTANCE*)handle;
    return pPhyInst == gPhyDrvInst ? pPhyInst : NULL;
}
#else
// multiple instances version
// could be refined more
DRV_ETHPHY_INSTANCE* F_ETHPHY_HandleToInst(void * handle)
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
#endif  // (M_DRV_ETHPHY_INSTANCES_NUMBER == 1)

void F_ETHPHY_SetOperDoneResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_RESULT res)
{
    hClientObj->operPhase =  hClientObj->operSubPhase = 0;
    hClientObj->operType = (uint16_t)PHY_CLIENT_OP_TYPE_NONE;
    hClientObj->status = (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY;
    hClientObj->operRes = (int16_t)res;
    if(res < (DRV_ETHPHY_RESULT)0)
    {   
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_OP_FAIL) != 0)
        SYS_CONSOLE_PRINT("DRV_PHY operation error: %d\r\n", res);
#endif  // ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_OP_FAIL) != 0)
    }
}

void F_ETHPHY_SetOperStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE opType, DRV_ETHPHY_RESULT res)
{
    hClientObj->operType = (uint16_t)opType;
    hClientObj->operPhase = hClientObj->operSubPhase = 0; 
    hClientObj->status = (int16_t)DRV_ETHPHY_CLIENT_STATUS_BUSY; 
    hClientObj->operRes = (int16_t)res;
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
     .phy_Initialize =             &DRV_ETHPHY_Initialize,
     .phy_Reinitialize =           &DRV_ETHPHY_Reinitialize,
     .phy_Deinitialize =           &DRV_ETHPHY_Deinitialize,
     .phy_Status =                 &DRV_ETHPHY_Status,
     .phy_Tasks =                  &DRV_ETHPHY_Tasks,
     .phy_Open =                   &DRV_ETHPHY_Open,
     .phy_Close =                  &DRV_ETHPHY_Close,
     .phy_ClientStatus =           &DRV_ETHPHY_ClientStatus,
     .phy_ClientOperationResult =  &DRV_ETHPHY_ClientOperationResult,
     .phy_ClientOperationAbort =   &DRV_ETHPHY_ClientOperationAbort,
     .phy_PhyAddressGet =          &DRV_ETHPHY_PhyAddressGet,
     .phy_Setup =                  &DRV_ETHPHY_Setup,
     .phy_RestartNegotiation =     &DRV_ETHPHY_RestartNegotiation,
     .phy_HWConfigFlagsGet =       &DRV_ETHPHY_HWConfigFlagsGet,
     .phy_NegotiationIsComplete =  &DRV_ETHPHY_NegotiationIsComplete,
     .phy_NegotiationResultGet =   &DRV_ETHPHY_NegotiationResultGet,
     .phy_LinkStatusGet =          &DRV_ETHPHY_LinkStatusGet,
     .phy_Reset =                  &DRV_ETHPHY_Reset,
     .phy_VendorDataGet =          &DRV_ETHPHY_VendorDataGet,
     .phy_VendorDataSet =          &DRV_ETHPHY_VendorDataSet,
     .phy_VendorSMIReadStart =     &DRV_ETHPHY_VendorSMIReadStart,
     .phy_VendorSMIReadResultGet = &DRV_ETHPHY_VendorSMIReadResultGet,
     .phy_VendorSMIWriteStart =    &DRV_ETHPHY_VendorSMIWriteStart,
     .phy_VendorSMIWriteWaitComplete = &DRV_ETHPHY_VendorSMIWriteWaitComplete,
     .phy_VendorSMIOperationIsComplete = &DRV_ETHPHY_VendorSMIOperationIsComplete,
};

// self reference to the base object itself
static const DRV_ETHPHY_OBJECT_BASE* gDrvEthBaseObj = &DRV_ETHPHY_OBJECT_BASE_Default;


//******************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_ETHPHY_Initialize( const SYS_MODULE_INDEX    iModule,
                                          const SYS_MODULE_INIT     * const init )

  Summary:
    Initializes hardware and data for the given instance of the ETHPHY module

  Description:
    This routine initializes hardware for the instance of the ETHPHY module,
    using the hardware initialization given data.  It also initializes all
    necessary internal data.

  Parameters:
    iModule        - Identifies the driver instance to be initialized
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

    if ( iModule >= (SYS_MODULE_INDEX)M_DRV_ETHPHY_INSTANCES_NUMBER )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    hSysObj = gPhyDrvInst + iModule;
    if( hSysObj->objInUse == (uint8_t)true )
    {   // already initalized
        return ( SYS_MODULE_OBJ )hSysObj ;
    }

    /* Assign to the local pointer the init data passed */
    ethphyInit = CF_ModInit2PhyInit(init);

    if((hSysObj->pMiimBase = ethphyInit->pMiimObject) == NULL)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    hSysObj->miimIndex = ethphyInit->miimIndex;

    hSysObj->objInUse = (uint8_t)true;      // Set object to be in use
    hSysObj->status = (int16_t)SYS_STATUS_READY; // Set module state
    hSysObj->iModule  = (uint16_t)iModule;  // Store driver instance
    hSysObj->ethphyId = ethphyInit->ethphyId; // Store PLIB ID

    // Assign External PHY Service Functions
    hSysObj->pPhyObj = ethphyInit->pPhyObject;
    hSysObj->pBaseObj = gDrvEthBaseObj;
    
    hSysObj->ethphyTmo = ethphyInit->ethphyTmo;
    
    hSysObj->objClient.clientInUse = 0;
    hSysObj->objClient.detectMask = hSysObj->pPhyObj->bmconDetectMask;
    if(hSysObj->objClient.detectMask == 0U)
    {   // use a default value...
       hSysObj->objClient.detectMask = (uint16_t)(PHY_BMCON_DETECT_MASK);
    }
    hSysObj->objClient.capabMask = hSysObj->pPhyObj->bmstatCpblMask;

    // hush compiler warning
    F_ETHPHY_AssertCond(true, __func__, __LINE__);
    M_PhyDebugCond(true, __func__, __LINE__);

    /* Return the driver handle */
    return ( SYS_MODULE_OBJ )hSysObj ;

}


void DRV_ETHPHY_Reinitialize( SYS_MODULE_OBJ        object ,
                              const SYS_MODULE_INIT * const init )
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(CF_SysObj2pVoid(object));
    if(phyInst != NULL)
    {
        DRV_ETHPHY_INIT * ethphyInit = CF_ModInit2PhyInit(init);

        phyInst->status = (int16_t)SYS_STATUS_READY; // Set module state
        phyInst->ethphyId = ethphyInit->ethphyId; // Store PLIB ID

        // Assign External PHY Service Functions
        phyInst->pPhyObj = ethphyInit->pPhyObject;

        phyInst->objClient.clientInUse = 0;
    }
}



void DRV_ETHPHY_Deinitialize( SYS_MODULE_OBJ object )
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(CF_SysObj2pVoid(object));
    if(phyInst != NULL)
    {
        /* Set the Device Status */
        phyInst->status  = (int16_t)SYS_STATUS_UNINITIALIZED;

        /* Remove the driver usage */
        phyInst->objInUse  = (uint8_t)false;

        phyInst->pPhyObj = NULL;
    }

} 


SYS_STATUS DRV_ETHPHY_Status( SYS_MODULE_OBJ object )
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(CF_SysObj2pVoid(object));
    if(phyInst != NULL)
    {
        /* Return the status associated with the driver handle */
        return (SYS_STATUS)phyInst->status;
    }

    return SYS_STATUS_ERROR;
} 


void DRV_ETHPHY_Tasks( SYS_MODULE_OBJ object )
{
    /* Check for the valid driver object passed */
    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(CF_SysObj2pVoid(object));
    if(phyInst != NULL)
    {
        DRV_ETHPHY_CLIENT_OBJ* hClientObj = &phyInst->objClient;

        if(hClientObj->clientInUse != 0U)
        {   // active client
            (*DRV_PHY_ClientOpTbl[hClientObj->operType])(hClientObj);
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
    if ( iModule >= (SYS_MODULE_INDEX)M_DRV_ETHPHY_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    phyInst = gPhyDrvInst + iModule; 
    if(phyInst->objInUse == (uint8_t)false)
    {   // not opened
        return DRV_HANDLE_INVALID;
    }

    /* Setup client operations */

    // Allocate the client object and set the flag as in use
    // there's only one client: the MAC
    hClientObj = (phyInst->objClient.clientInUse == 0U) ?  &phyInst->objClient : (DRV_ETHPHY_CLIENT_OBJ *)0;

    while(hClientObj != NULL)
    {
        DRV_HANDLE miimHandle = phyInst->pMiimBase->miim_Open(phyInst->miimIndex, DRV_IO_INTENT_SHARED);
        if(miimHandle == DRV_HANDLE_INVALID)
        {
            M_PhyDebugCond(false, __func__, __LINE__);
            break;
        }

        // set the MIIM driver so it can be used by the PHY setup procedure
        DRV_MIIM_SETUP  miimSetup;

        union
        {
            int32_t     i32;
            uint32_t    u32;
        }U_I32_U32;
        U_I32_U32.i32 = TCPIP_INTMAC_PERIPHERAL_CLK;
        miimSetup.hostClockFreq = U_I32_U32.u32;


        miimSetup.maxBusFreq = (*phyInst->pPhyObj->smiClockGet)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj);
        miimSetup.setupFlags = DRV_MIIM_SETUP_FLAG_NONE;

        DRV_MIIM_RESULT miimRes = phyInst->pMiimBase->miim_Setup(miimHandle, &miimSetup); 
        if(miimRes < (DRV_MIIM_RESULT)0)
        {   // error occurred
            phyInst->pMiimBase->miim_Close( miimHandle);
            M_PhyDebugCond(false, __func__, __LINE__);
            return DRV_HANDLE_INVALID;
        }

        hClientObj->pMiimBase = phyInst->pMiimBase;
        hClientObj->miimHandle = miimHandle;
        hClientObj->miimOpHandle = NULL;

        hClientObj->clientInUse    = (uint8_t)true;
        hClientObj->hDriver  = phyInst;
        hClientObj->ethphyId = phyInst->ethphyId;
        hClientObj->operType = (uint16_t)PHY_CLIENT_OP_TYPE_NONE;
        hClientObj->smiTxferStatus = (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_NONE;
        F_DRV_PHY_SetOperPhase(hClientObj, 0, 0);

        /* Update the Client Status */
        hClientObj->status = (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY;

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

    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            hClientObj->pMiimBase->miim_Close(hClientObj->miimHandle);

            /* Free the Client Instance */
            hClientObj->clientInUse = (uint8_t)false ;

            /* Update the Client Status */
            hClientObj->status = (int16_t)DRV_ETHPHY_CLIENT_STATUS_CLOSED;
        }
    }
}

DRV_ETHPHY_RESULT DRV_ETHPHY_Setup( DRV_HANDLE handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
{

    const DRV_ETHPHY_OBJECT* pPhyObj;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
    if(phyInst == NULL)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    // basic sanity check
    if((pSetUp == NULL) || (pSetupFlags == NULL))
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    // check the PHY set up parameters
    pPhyObj = phyInst->pPhyObj;

    if((pPhyObj == NULL) || (pPhyObj->miiConfigure == NULL) || (pPhyObj->mdixConfigure == NULL) || (pPhyObj->smiClockGet == NULL))
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if(pSetUp->resetFunction != NULL)
    {
        (*pSetUp->resetFunction)(gDrvEthBaseObj, handle);
    }

    phyInst->phyAddress = (uint16_t)(pSetUp->phyAddress);
    phyInst->openFlags = (uint32_t)(pSetUp->openFlags);
    phyInst->configFlags = (uint16_t)(pSetUp->configFlags);
    phyInst->macPauseType = (uint8_t)(pSetUp->macPauseType);
    
    hClientObj->operParam = (uintptr_t)pSetupFlags;
    F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_SETUP, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}

// PHY_CLIENT_OP_TYPE_NONE
static void F_ETHPHY_ClientOpNone(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
}

// PHY_CLIENT_OP_TYPE_SETUP
static void F_ETHPHY_ClientOpSetup(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if((unsigned int)hClientObj->operPhase >= sizeof(DRV_PHY_SetupPhasesTbl)/sizeof(*DRV_PHY_SetupPhasesTbl))
    {   // shouldn't happen
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
    else
    {
        (*DRV_PHY_SetupPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
}


static void F_DRV_ETHPHY_SetupPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    DRV_ETHPHY_INSTANCE * hDriver = NULL;
    uint16_t configFlags = (uint16_t)DRV_ETHPHY_CFG_DEFAULT;
    uint32_t openFlags = (uint32_t)TCPIP_ETH_OPEN_DEFAULT;      // flags required at open time

    hDriver = hClientObj->hDriver;

    configFlags = hDriver->configFlags;

#if defined(__PIC32MX__) || defined(__PIC32MZ__)
    // get the way the hw is configured
    DRV_ETHPHY_CONFIG_FLAGS getCfgFlags = DRV_ETHPHY_CFG_DEFAULT;
    (void)DRV_ETHPHY_HWConfigFlagsGet((DRV_HANDLE)hClientObj, &getCfgFlags);
    uint16_t hwFlags = (uint16_t)getCfgFlags;

    if((configFlags & (uint16_t)DRV_ETHPHY_CFG_AUTO) != 0U)
    {
        hDriver->configFlags = configFlags = hwFlags;
    }
    else
    {   // some minimal check against the way the hw is configured
        uint16_t swFlags = configFlags & ((uint16_t)DRV_ETHPHY_CFG_RMII|(uint16_t)DRV_ETHPHY_CFG_ALTERNATE);

        if((swFlags ^ hwFlags) != 0U)
        {   // hw-sw configuration mismatch MII/RMII, ALT/DEF config
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_CFG_ERR);
        }
    }
#endif

    openFlags = hDriver->openFlags;
    if((openFlags & ((uint32_t)TCPIP_ETH_OPEN_PHY_LOOPBACK | (uint32_t)TCPIP_ETH_OPEN_MAC_LOOPBACK)) != 0U)
    {
        openFlags &= ~(uint32_t)TCPIP_ETH_OPEN_AUTO; // no negotiation in loopback mode!
    }

    if((openFlags & (uint32_t)TCPIP_ETH_OPEN_AUTO) == 0U)
    {
        openFlags &= ~(uint32_t)TCPIP_ETH_OPEN_MDIX_AUTO;        // Auto-MDIX has to be in auto negotiation only
    }

    if ((configFlags & (uint16_t)DRV_ETHPHY_CFG_GMII)!=0U)
    {
        openFlags |= (uint32_t)TCPIP_ETH_OPEN_GMII;
    }
    else if ((configFlags & (uint16_t)DRV_ETHPHY_CFG_RGMII)!=0U)
    {
        openFlags |= (uint32_t)TCPIP_ETH_OPEN_RGMII;
    }
    else if ((configFlags & (uint16_t)DRV_ETHPHY_CFG_MII)!=0U)
    {
        openFlags |= (uint32_t)TCPIP_ETH_OPEN_MII;
    }
    else if ((configFlags & (uint16_t)DRV_ETHPHY_CFG_RMII)!=0U)
    {
        openFlags |= (uint32_t)TCPIP_ETH_OPEN_RMII;
    }
    else
    {
        //Do Nothing
    }
    
    hDriver->openFlags = openFlags;

    F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_DETECT, 0);
}

    

static void F_DRV_ETHPHY_SetupPhaseDetect(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_VENDOR_DETECT detectF;
    DRV_ETHPHY_INSTANCE* hDriver;
    DRV_ETHPHY_RESULT res; 
    uint32_t temp_tickFreq = 0;

    switch (hClientObj->operSubPhase)
    {
        case 0U:
            // set the function to perform the transfer
            hDriver = hClientObj->hDriver;

            if((detectF = hDriver->pPhyObj->phyDetect) == NULL)
            {   // call the default detect procedure
                detectF = &F_ETHPHY_DefaultDetect;
            } 

            hClientObj->vendorDetect = detectF;
            hClientObj->vendorData = 0;
            temp_tickFreq = SYS_TMR_TickCounterFrequencyGet();
            hClientObj->operTStamp = SYS_TMR_TickCountGet() + (uint32_t)((((hDriver->ethphyTmo->resetTmo) * temp_tickFreq) + 999U) / 1000U);
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_DETECT, 1);
            break;

        case 1U:
            // perform the transfer
            res =  hClientObj->vendorDetect(gDrvEthBaseObj, (DRV_HANDLE)hClientObj);
            if(res == DRV_ETHPHY_RES_OK)
            {   // everything seems to be fine
                // initiate the PHY ID Read
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_READ_ID, 0);
            }
            else if(res < (DRV_ETHPHY_RESULT)0)
            {   // failed 
                F_ETHPHY_SetOperDoneResult(hClientObj, res);
            }
            else
            {   // DRV_ETHPHY_RES_PENDING wait some more...
                if(SYS_TMR_TickCountGet() >= hClientObj->operTStamp)
                {   // tmo occurred
                    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_TMO);
                }
            }
            break;

        default:
            // shouldn't happen
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
            break;
    }

}


// default PHY detection procedure
// the DRV_ETHPHY_OBJECT can specify a BMCON detectMask
//      if this mask == 0, then the default procedure will use the BMCON.RESET and BMCON.LOOPBACK bits
//      i.e. the corresponding BMCON_LOOPBACK_MASK | BMCON_DUPLEX_MASK BMCON mask
// else the DRV_ETHPHY_OBJECT::bmconDetectMask will be used!
static DRV_ETHPHY_RESULT F_ETHPHY_DefaultDetect( const struct DRV_ETHPHY_OBJECT_BASE_TYPE* pBaseObj, DRV_HANDLE hClientObj)
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

    BMCONbits_t bmcon;
    uint16_t    detectMask;
    uint16_t    phyReg = 0;
    uint16_t    detectPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->phy_VendorDataGet(hClientObj, &vendorData.w);

    if(res < (DRV_ETHPHY_RESULT)0)
    {   // some error occurred
        return res;
    }
    else
    {
        //Do Nothing
    }

    (void)pBaseObj->phy_PhyAddressGet(hClientObj, DRV_ETHPHY_INF_IDX_ALL_EXTERNAL, &phyAddress);

    detectPhase = vendorData.low;

    F_ETHPHY_Dbg_DetectPhase(detectPhase);

    // try to detect the PHY and reset it

    switch (detectPhase)
    {
        case 0U:
            // initiate a read of the BMCON register
            res = pBaseObj->phy_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
            if(res < (DRV_ETHPHY_RESULT)0)
            {   // some error
                // Do Nothing
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                res = DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                // advance to the next phase
                detectPhase++;
                vendorData.low = detectPhase;
                (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;

        case 1U:
            // wait the BMCON read to complete
            res = pBaseObj->phy_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < (DRV_ETHPHY_RESULT)0)
            {   // some error
                // Do Nothing
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                res =  DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                // got BMCON result
                bmcon.w =  phyReg;
                F_ETHPHY_Dbg_DetectReadValue(PHY_REG_BMCON, bmcon.w, BMCON_RESET_MASK, 0);
                if(bmcon.RESET == 0U)
                {   // all good;advance to the next phase
                    detectPhase++;
                    vendorData.low = detectPhase;
                    (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                    res =  DRV_ETHPHY_RES_PENDING;
                }
                else
                {
                    // that is suspicios...but give it a chance to clear itself
                    // otherwise operation will timeout
                    // back from start: re-read BMCON
                    vendorData.w = 0;
                    (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                    res = DRV_ETHPHY_RES_PENDING;
                }                
            }
            break;

        case 2U:
            // try to see if we can write smth to the PHY
            bmcon.w = ((DRV_ETHPHY_CLIENT_OBJ*)hClientObj)->detectMask;
            res = pBaseObj->phy_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, bmcon.w, phyAddress);
            F_ETHPHY_Dbg_DetectWriteValue(PHY_REG_BMCON, bmcon.w);
            if(res < (DRV_ETHPHY_RESULT)0)
            {   // some error
                // Do Nothing
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                res = DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                detectPhase++;
                vendorData.low = detectPhase;
                (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;

        case 3U:
            // initiate re-read the BMCON
            res = pBaseObj->phy_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
            if(res < (DRV_ETHPHY_RESULT)0)
            {   // some error
                // Do Nothing
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                res = DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                // advance to the next phase
                detectPhase++;
                vendorData.low = detectPhase;
                (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;            

        case 4U:
            // wait the BMCON read to complete
            res = pBaseObj->phy_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < (DRV_ETHPHY_RESULT)0)
            {   // some error
                // Do Nothing
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                res = DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                bmcon.w = phyReg;
                detectMask = ((DRV_ETHPHY_CLIENT_OBJ*)hClientObj)->detectMask;
                F_ETHPHY_Dbg_DetectReadValue(PHY_REG_BMCON, bmcon.w, detectMask, detectMask);
                if((bmcon.w & detectMask) != detectMask)
                {   // failed to set
                    res = DRV_ETHPHY_RES_DTCT_ERR; 
                }
                else
                {
                    // advance to the next phase
                    vendorData.high = phyReg;
                    detectPhase++;
                    vendorData.low = detectPhase;
                    (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                    res = DRV_ETHPHY_RES_PENDING;
                }
            }
            break;     
            
        case 5U:
            // clear bits and write
            bmcon.w = vendorData.high;
            bmcon.w ^= ((DRV_ETHPHY_CLIENT_OBJ*)hClientObj)->detectMask;
            res = pBaseObj->phy_VendorSMIWriteStart(hClientObj, PHY_REG_BMCON, bmcon.w, phyAddress);
            F_ETHPHY_Dbg_DetectWriteValue(PHY_REG_BMCON, bmcon.w);
            if(res < (DRV_ETHPHY_RESULT)0)
            {   // some error
                // Do Nothing
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                res = DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                // advance to the next phase
                detectPhase++;
                vendorData.low = detectPhase;
                (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;

        case 6U:
            // re-read BMCON
            res = pBaseObj->phy_VendorSMIReadStart(hClientObj, PHY_REG_BMCON, phyAddress);
            if(res < (DRV_ETHPHY_RESULT)0)
            {   // some error
                // Do Nothing
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                res = DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                // advance to the next phase
                detectPhase++;
                vendorData.low = detectPhase;
                (void)pBaseObj->phy_VendorDataSet(hClientObj, vendorData.w);
                res = DRV_ETHPHY_RES_PENDING;
            }
            break;

        case 7U:
            // wait read complete
            res = pBaseObj->phy_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < (DRV_ETHPHY_RESULT)0)
            {   // some error
                // Do Nothing
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                res = DRV_ETHPHY_RES_PENDING;
            }
            else
            {
                bmcon.w = phyReg;
                detectMask = ((DRV_ETHPHY_CLIENT_OBJ*)hClientObj)->detectMask;
                F_ETHPHY_Dbg_DetectReadValue(PHY_REG_BMCON, bmcon.w, detectMask, 0);
                if((bmcon.w & detectMask) != 0U)
                {   // failed to clear
                    res = DRV_ETHPHY_RES_DTCT_CLR_ERR;
                }
                else
                {
                    // everything seems to be fine
                    res = DRV_ETHPHY_RES_OK;
                }
            }
            break;
            
        default:
            // shouldn't happen
            res = DRV_ETHPHY_RES_OPERATION_ERR;
            break;
    }
    return res;
}

// performs the PHY reset
static void F_DRV_ETHPHY_SetupPhaseReadId(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    switch(hClientObj->operSubPhase)
    {
        case 0U:
            // read the PHY Identifier 1 (2h) register
            if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_PHYID1))
            {
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_READ_ID, 1);
            }
            break;
        case 1U:
            // wait the register read to complete
            if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
            {
                break;
            }     
            hClientObj->phyId_1 = hClientObj->smiData;
            
            // read the PHY Identifier 2 (3h) register
            if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_PHYID2))
            {
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_READ_ID, 2);
            }
            break;

        case 2U:
            // wait the register read to complete
            if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
            {
                break;
            }

            hClientObj->phyId_2 = hClientObj->smiData;
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_RESET, 0);
            break;

        default:
            // shouldn't happen
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
            break;
    }

}

// performs the PHY reset
static void F_DRV_ETHPHY_SetupPhaseReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMCONbits_t bmcon;
    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
    
    switch(hClientObj->operSubPhase)
    {
        case 0U:
            // initiate Soft Reset the PHY
            if(F_DRV_PHY_SMIWriteStart(hClientObj, (uint16_t)PHY_REG_BMCON, (uint16_t)BMCON_RESET_MASK))
            {
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_RESET, 1);
                uint32_t temp_tickFreq = SYS_TMR_TickCounterFrequencyGet();
                hClientObj->operTStamp = SYS_TMR_TickCountGet() + (uint32_t)((((phyInst->ethphyTmo->resetTmo) * temp_tickFreq) + 999U) / 1000U);
            }
            break;

        case 1U:
            // read the BMCON
            if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMCON))
            {
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_RESET, 2);
            }
            break;

        case 2U:
            // wait the BMCON write/read to complete
            if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
            {
                break;
            }

            bmcon.w = hClientObj->smiData;
            if(bmcon.RESET != 0U)
            {   // wait to clear itself
                if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
                {   // not yet tmo; re-read
                    F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_RESET, 1);
                }
                else
                {
                    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_RST_CLR_ERR);
                }
            }
            else
            {   // success
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 0);
            }
            break;

        default:
            // shouldn't happen
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
            break;
    }

}

static void F_SetupPhaseNeg_SubPhase_Start(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_Read_Std_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_Read_Ext_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_MII_Config(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_MDIX_Config(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_ANAD_Write(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_Read_1000B_Ctrl(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_1000BCtrl_Write(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_Read_AutoNeg_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_Start_AutoNeg(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void F_SetupPhaseNeg_SubPhase_Finish(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

typedef enum
{
    SETUP_NEG_SUBPHASE_START = 0,
    SETUP_NEG_SUBPHASE_READ_STD_STATUS,
    SETUP_NEG_SUBPHASE_READ_EXT_STATUS,
    SETUP_NEG_SUBPHASE_CONFIG_MII,
    SETUP_NEG_SUBPHASE_CONFIG_MDIX,
    SETUP_NEG_SUBPHASE_WRITE_ANAD,
    SETUP_NEG_SUBPHASE_READ_1000B_CTRL,
    SETUP_NEG_SUBPHASE_WRITE_1000B_CTRL,
    SETUP_NEG_SUBPHASE_READ_AN_STATUS,
    SETUP_NEG_SUBPHASE_START_AN,
    SETUP_NEG_SUBPHASE_FINISH,
} DRV_ETHPHY_SETUP_NEG_PHASE;


static const DRV_PHY_OperPhaseF_T DRV_PHY_SetupPhaseNegotiateSubPhaseTbl[] = 
{
    &F_SetupPhaseNeg_SubPhase_Start,
    &F_SetupPhaseNeg_SubPhase_Read_Std_Status,
    &F_SetupPhaseNeg_SubPhase_Read_Ext_Status,
    &F_SetupPhaseNeg_SubPhase_MII_Config,
    &F_SetupPhaseNeg_SubPhase_MDIX_Config,
    &F_SetupPhaseNeg_SubPhase_ANAD_Write,
    &F_SetupPhaseNeg_SubPhase_Read_1000B_Ctrl,
    &F_SetupPhaseNeg_SubPhase_1000BCtrl_Write,
    &F_SetupPhaseNeg_SubPhase_Read_AutoNeg_Status,
    &F_SetupPhaseNeg_SubPhase_Start_AutoNeg,
    &F_SetupPhaseNeg_SubPhase_Finish,
};

static void F_DRV_ETHPHY_SetupPhaseNegotiate(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(hClientObj->operSubPhase <  sizeof(DRV_PHY_SetupPhaseNegotiateSubPhaseTbl) / sizeof(*DRV_PHY_SetupPhaseNegotiateSubPhaseTbl))
    {
        (*DRV_PHY_SetupPhaseNegotiateSubPhaseTbl[hClientObj->operSubPhase])(hClientObj);
    } 
    else
    {   // shouldn't happen
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

// SETUP_NEG_SUBPHASE_START
static void F_SetupPhaseNeg_SubPhase_Start(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    uint32_t openFlags;      // flags required at open time
    
    // provide some defaults
    openFlags = hClientObj->hDriver->openFlags;
    if((openFlags & ((uint32_t)TCPIP_ETH_OPEN_FDUPLEX|(uint32_t)TCPIP_ETH_OPEN_HDUPLEX))== 0U)
    {
        openFlags |= (uint32_t)TCPIP_ETH_OPEN_HDUPLEX;
    }
    if((openFlags & ((uint32_t)TCPIP_ETH_OPEN_100 | (uint32_t)TCPIP_ETH_OPEN_10 | (uint32_t)TCPIP_ETH_OPEN_1000)) == 0U)
    {
        openFlags |= (uint32_t)TCPIP_ETH_OPEN_10;
    }

    hClientObj->hDriver->openFlags = openFlags;   
    // try to match the openFlags with the PHY capabilities
    // initiate the read of the PHY capabilities
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_READ_STD_STATUS);
    }

}
    
// SETUP_NEG_SUBPHASE_READ_STD_STATUS
static void F_SetupPhaseNeg_SubPhase_Read_Std_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    uint32_t  openFlags;      // flags required at open time
    uint16_t  stdReqs = 0;
    uint16_t  phyBmstatCpbl;
    uint16_t  matchStdCpbl = 0;
    
    // wait the BMCON PHY_REG_BMSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }
    phyBmstatCpbl = hClientObj->smiData; // BMSTAT value
    phyBmstatCpbl |= hClientObj->capabMask; // use vendor bmstatCpblMask 
    
    // provide some defaults
    openFlags = hClientObj->hDriver->openFlags;
    if((openFlags & ((uint32_t)TCPIP_ETH_OPEN_FDUPLEX | (uint32_t)TCPIP_ETH_OPEN_HDUPLEX)) == 0U)
    {
        openFlags |= (uint32_t)TCPIP_ETH_OPEN_HDUPLEX;
    }
    if((openFlags & ((uint32_t)TCPIP_ETH_OPEN_100 | (uint32_t)TCPIP_ETH_OPEN_10 | (uint32_t)TCPIP_ETH_OPEN_1000)) == 0U)
    {
        openFlags |= (uint32_t)TCPIP_ETH_OPEN_10;
    }

    if((openFlags & (uint32_t)TCPIP_ETH_OPEN_AUTO) != 0U)
    {   // advertise auto negotiation
        stdReqs = (uint16_t)BMSTAT_AN_ABLE_MASK;
                
        if((openFlags & (uint32_t)TCPIP_ETH_OPEN_100) != 0U)
        {
            if((openFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX) != 0U)
            {
                stdReqs |= (uint16_t)BMSTAT_BASE100TX_FDX_MASK;
            }
            if((openFlags & (uint32_t)TCPIP_ETH_OPEN_HDUPLEX) != 0U)
            {
                stdReqs |= (uint16_t)BMSTAT_BASE100TX_HDX_MASK;
            }
        }

        if((openFlags & (uint32_t)TCPIP_ETH_OPEN_10)!= 0U)
        {
            if((openFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX) != 0U)
            {
                stdReqs |= (uint16_t)BMSTAT_BASE10T_FDX_MASK;
            }
            if((openFlags & (uint32_t)TCPIP_ETH_OPEN_HDUPLEX) != 0U)
            {
                stdReqs |= (uint16_t)BMSTAT_BASE10T_HDX_MASK;
            }
        }
    }
    else
    {   // no auto negotiation
        if((openFlags & (uint32_t)TCPIP_ETH_OPEN_100) != 0U)
        {
            stdReqs = ((openFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX)!= 0U) ? (uint16_t)BMSTAT_BASE100TX_FDX_MASK : (uint16_t)BMSTAT_BASE100TX_HDX_MASK;
        }
        else
        {
            stdReqs = ((openFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX)!= 0U) ? (uint16_t)BMSTAT_BASE10T_FDX_MASK : (uint16_t)BMSTAT_BASE10T_HDX_MASK;
        }
    }

    // Standard Capabilities of PHY compared with Standard features Requested in configuration 
    matchStdCpbl = ((stdReqs & ((uint16_t)PHY_STD_CPBL_MASK | (uint16_t)BMSTAT_AN_ABLE_MASK))| (uint16_t)BMSTAT_EXTSTAT_MASK | (uint16_t)BMSTAT_EXTEND_ABLE_MASK) & phyBmstatCpbl; 
    
    if((matchStdCpbl == 0U) && ((phyBmstatCpbl & (uint16_t)BMSTAT_EXTSTAT_MASK)==0U) )
    {   // no match?
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_CPBL_ERR);
        return;
    }

    // we're ok, we can configure the PHY
    hClientObj->hDriver->openFlags = openFlags;
    hClientObj->operReg[0] = matchStdCpbl;
    hClientObj->vendorData = 0;
    
    // Check Phy is extended register capable and extended status capable
    if (((phyBmstatCpbl & (uint16_t)BMSTAT_EXTSTAT_MASK) != 0U) && ((phyBmstatCpbl & (uint16_t)BMSTAT_EXTEND_ABLE_MASK) != 0U))
    {
        if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_EXTSTAT))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_READ_EXT_STATUS);
        }
    }
    else
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_CONFIG_MII);
    }

}


// SETUP_NEG_SUBPHASE_READ_EXT_STATUS
static void F_SetupPhaseNeg_SubPhase_Read_Ext_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    uint32_t  openFlags;      // flags required at open time
    static uint16_t phyExtCpbl = 0;
    uint16_t  matchExtCpbl = 0;
    uint16_t  extReqs = 0;
    
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }
    phyExtCpbl = hClientObj->smiData;
    
    // provide some defaults
    openFlags = hClientObj->hDriver->openFlags;   

    if((openFlags & (uint32_t)TCPIP_ETH_OPEN_AUTO) != 0U)
    {
        if((openFlags & (uint32_t)TCPIP_ETH_OPEN_1000) != 0U)
        {
            if((openFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX) != 0U)
            {
                extReqs |= (uint16_t)EXTSTAT_1000BASET_FDX_MASK;
            }
            if((openFlags & (uint32_t)TCPIP_ETH_OPEN_HDUPLEX) != 0U)
            {
                extReqs |= (uint16_t)EXTSTAT_1000BASET_HDX_MASK;
            }
        }     
    }
    else
    {   // no auto negotiation
        if((openFlags & (uint32_t)TCPIP_ETH_OPEN_1000) != 0U)
        {
            extReqs = ((openFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX) != 0U)? (uint16_t)EXTSTAT_1000BASET_FDX_MASK : (uint16_t)EXTSTAT_1000BASET_HDX_MASK;
        }
    }
    
    matchExtCpbl = (extReqs & PHY_EXT_CPBL_MASK) & phyExtCpbl;
    
    hClientObj->operReg[1] = matchExtCpbl;
    hClientObj->vendorData = 0;
    F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_CONFIG_MII);  
    
}

// MII/RMII configuration phase
// SETUP_NEG_SUBPHASE_CONFIG_MII
static void F_SetupPhaseNeg_SubPhase_MII_Config(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_INSTANCE *hDriver;
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;

    hDriver = hClientObj->hDriver;
    res = (*hDriver->pPhyObj->miiConfigure)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj, hDriver->configFlags);

    if(res == DRV_ETHPHY_RES_OK)
    {   // success, advance
        // next phase: configure the MDIX
        hClientObj->vendorData = 0;
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_CONFIG_MDIX);
    }
    else if(res != DRV_ETHPHY_RES_PENDING)
    {   // some error occurred
        F_ETHPHY_SetOperDoneResult(hClientObj, res);
    }
    else
    {
        //Do Nothing
    }

}


// MDIX configure
// SETUP_NEG_SUBPHASE_CONFIG_MDIX
static void F_SetupPhaseNeg_SubPhase_MDIX_Config(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_RESULT res;
    DRV_ETHPHY_INSTANCE *hDriver = hClientObj->hDriver;

    res = (*hDriver->pPhyObj->mdixConfigure)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj, hClientObj->hDriver->openFlags);

    if(res < (DRV_ETHPHY_RESULT)0)
    {   // some error occurred
        F_ETHPHY_SetOperDoneResult(hClientObj, res);
        return;
    }
    else if(res != DRV_ETHPHY_RES_OK)
    {   // still waiting
        return;
    }
    else
    {
        //Do Nothing
    }

    // success, advance;
    F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_WRITE_ANAD);
}


// SETUP_NEG_SUBPHASE_WRITE_ANAD
static void F_SetupPhaseNeg_SubPhase_ANAD_Write(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_INSTANCE *hDriver = hClientObj->hDriver;
    // restore match capabilities
    uint16_t  matchStdCpbl = hClientObj->operReg[0];
    uint32_t  openFlags = hClientObj->hDriver->openFlags;

    if(((matchStdCpbl & (uint16_t)BMSTAT_AN_ABLE_MASK) != 0U) && ((openFlags & (uint32_t)TCPIP_ETH_OPEN_AUTO) != 0U))
    {   // ok, we can perform auto negotiation
        uint16_t anadReg = 0;

        anadReg = ((uint16_t)((matchStdCpbl >> BMSTAT_CAPABILITY_POS) << ANAD_CAPABLITY_POS) & (uint16_t)ANAD_NEGOTIATION_MASK) | (uint16_t)PHY_PROT_802_3;
        
        //Errata: Do not set asymmetric pause for KSZ9031 PHY        
        if((hDriver->macPauseType & (uint8_t)TCPIP_ETH_PAUSE_TYPE_PAUSE) != 0U)
        {
            anadReg |= (uint16_t)ANAD_PAUSE_MASK;
        }
        if (((hClientObj->phyId_1 == (uint16_t)KSZ9031_PHYID1) && (((hClientObj->phyId_2)&((uint16_t)KSZ9031_PHYID2)) == (uint16_t)KSZ9031_PHYID2)) == false)
        {    
            if((hDriver->macPauseType & (uint8_t)TCPIP_ETH_PAUSE_TYPE_ASM_DIR) != 0U)
            {
               anadReg |= (uint16_t)ANAD_ASM_DIR_MASK;
            } 
        }

        // advertise our capabilities
        if(F_DRV_PHY_SMIWriteStart(hClientObj, (uint16_t)PHY_REG_ANAD, anadReg))
        {   // check for extended capabilities
            if (((matchStdCpbl & (uint16_t)BMSTAT_EXTSTAT_MASK)!= 0U) && ((matchStdCpbl & (uint16_t)BMSTAT_EXTEND_ABLE_MASK)!= 0U))
            {
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_READ_1000B_CTRL);
            }
            else
            {
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_READ_AN_STATUS);
            }
        }
    }
    else
    {   // ok, just don't use negotiation
        uint16_t  ctrlReg;

        ctrlReg = 0;
        // set 100Mbps request/capability
        if(((matchStdCpbl & ((uint16_t)BMSTAT_BASE100TX_HDX_MASK | (uint16_t)BMSTAT_BASE100TX_FDX_MASK))!= 0U) && ((openFlags & (uint32_t)TCPIP_ETH_OPEN_100) != 0U))  
        {
            ctrlReg |= (uint16_t)BMCON_SPEED100_MASK;
        }

        if((matchStdCpbl & ((uint16_t)BMSTAT_BASE10T_FDX_MASK | (uint16_t)BMSTAT_BASE100TX_FDX_MASK ))!= 0U)
        {
            ctrlReg |= (uint16_t)BMCON_DUPLEX_MASK;
        }

        if((hClientObj->hDriver->openFlags & (uint32_t)TCPIP_ETH_OPEN_PHY_LOOPBACK) != 0U)
        {
            ctrlReg |= (uint16_t)BMCON_LOOPBACK_MASK;
        }

        if(F_DRV_PHY_SMIWriteStart(hClientObj, (uint16_t)PHY_REG_BMCON, ctrlReg))
        {   
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_FINISH);
        }
    }

}

// SETUP_NEG_SUBPHASE_READ_1000B_CTRL
static void F_SetupPhaseNeg_SubPhase_Read_1000B_Ctrl(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_1000BASECON))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_WRITE_1000B_CTRL);
    }    
    
}

// SETUP_NEG_SUBPHASE_WRITE_1000B_CTRL
static void F_SetupPhaseNeg_SubPhase_1000BCtrl_Write(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    uint16_t ext1000baseCtrl = 0, matchExtStat = 0;
    
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }
    ext1000baseCtrl = hClientObj->smiData;
    matchExtStat = hClientObj->operReg[1];
    
    if((matchExtStat & (uint16_t)EXTSTAT_1000BASET_FDX_MASK) != 0U)
    {
        ext1000baseCtrl |= (uint16_t)TCTRL_1000BASE_TFDX_MASK;
    }
    else
    {
        ext1000baseCtrl &= ~(uint16_t)TCTRL_1000BASE_TFDX_MASK;
    }
    if((matchExtStat & (uint16_t)EXTSTAT_1000BASET_HDX_MASK) != 0U)
    {
        ext1000baseCtrl |= (uint16_t)TCTRL_1000BASE_THDX_MASK;
    }
    else
    {
        ext1000baseCtrl &= ~(uint16_t)TCTRL_1000BASE_THDX_MASK;
    }
    
    if(F_DRV_PHY_SMIWriteStart( hClientObj, (uint16_t)PHY_REG_1000BASECON, ext1000baseCtrl))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_READ_AN_STATUS);
    }
    
}

// advertise, negotiation restart phase
// SETUP_NEG_SUBPHASE_READ_AN_STATUS
static void F_SetupPhaseNeg_SubPhase_Read_AutoNeg_Status(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT))
    {   // read capabilities initiate
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_START_AN);
    }
}
    
// auto-negotiation perform
// SETUP_NEG_SUBPHASE_START_AN
static void F_SetupPhaseNeg_SubPhase_Start_AutoNeg(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the PHY_REG_BMSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    BMSTATbits_t  phyStdCpbl;

    phyStdCpbl.w = hClientObj->smiData;

    if(phyStdCpbl.AN_ABLE != 0U)
    {   // ok, we can perform auto negotiation
        // restart negotiation and we'll have to wait
        if(F_DRV_PHY_SMIWriteStart( hClientObj, (uint16_t)PHY_REG_BMCON, (uint16_t)BMCON_AN_ENABLE_MASK | (uint16_t)BMCON_AN_RESTART_MASK))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, (uint16_t)SETUP_NEG_SUBPHASE_FINISH);
        }
    }
    else
    {   // no negotiation ability!
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_UNABLE);
    }
}

// final update stage
// SETUP_NEG_SUBPHASE_FINISH
static void F_SetupPhaseNeg_SubPhase_Finish(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    uint32_t openFlags = (uint32_t)TCPIP_ETH_OPEN_DEFAULT;
    uint16_t  matchStdCpbl = hClientObj->operReg[0];
    uint16_t  matchExtCpbl = hClientObj->operReg[1];
    
    // now update the open flags
    // the upper layer needs to know the PHY set-up to further set-up the MAC.
    openFlags = hClientObj->hDriver->openFlags;


    if((matchStdCpbl & (uint16_t)BMSTAT_AN_ABLE_MASK) == 0U)
    {       
        openFlags &= ~(uint32_t)TCPIP_ETH_OPEN_AUTO;
    }
    
    // set 1000Mbps request/capability
    if((matchExtCpbl & ((uint16_t)EXTSTAT_1000BASET_FDX_MASK | (uint16_t)EXTSTAT_1000BASET_HDX_MASK)) == 0U) 
    {
        openFlags &= ~(uint32_t)TCPIP_ETH_OPEN_1000;
    }
    
    // set 100Mbps request/capability
    if((matchStdCpbl & ((uint16_t)BMSTAT_BASE100TX_HDX_MASK | (uint16_t)BMSTAT_BASE100TX_FDX_MASK)) == 0U)  
    {
        openFlags &= ~(uint32_t)TCPIP_ETH_OPEN_100;
    }
    
    // set 10Mbps request/capability
    if((matchStdCpbl & ((uint16_t)BMSTAT_BASE10T_HDX_MASK | (uint16_t)BMSTAT_BASE10T_FDX_MASK)) == 0U)  
    {
        openFlags &= ~(uint32_t)TCPIP_ETH_OPEN_10;
    }
    
    if((((matchStdCpbl & ((uint16_t)BMSTAT_BASE10T_FDX_MASK | (uint16_t)BMSTAT_BASE100TX_FDX_MASK)) != 0U) || ((matchExtCpbl & (uint16_t)BASE1000T_FDX_MASK ) != 0U)) == false)
    {
        openFlags &= ~(uint32_t)TCPIP_ETH_OPEN_FDUPLEX;
    }
    
    if((((matchStdCpbl & ((uint16_t)BMSTAT_BASE10T_HDX_MASK | (uint16_t)BMSTAT_BASE100TX_HDX_MASK)) != 0U) || ((matchExtCpbl & (uint16_t)BASE1000T_HDX_MASK ) != 0U)) == false)
    {
        openFlags &= ~(uint32_t)TCPIP_ETH_OPEN_HDUPLEX;
    }
   
    // store the openFlags!
    // upper layer needs to know the PHY set-up to further set-up the MAC.
    hClientObj->hDriver->openFlags = openFlags;

    // we're done
    // deposit result
    TCPIP_ETH_OPEN_FLAGS* pSetUpFlags = (TCPIP_ETH_OPEN_FLAGS*)hClientObj->operParam;
    *pSetUpFlags = (TCPIP_ETH_OPEN_FLAGS)openFlags;
     F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);

} 

DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_ClientStatus( DRV_HANDLE handle )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            /* Return the client status associated with the handle passed */
            return(DRV_ETHPHY_CLIENT_STATUS)hClientObj->status;
        }
    }

    return DRV_ETHPHY_CLIENT_STATUS_ERROR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationResult( DRV_HANDLE handle)  
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            /* Return the client status associated with the handle passed */
            return(DRV_ETHPHY_RESULT)(hClientObj->operRes);
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationAbort( DRV_HANDLE handle)  
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_ABORTED);
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
    if( hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(pPhyAddress != NULL)
            {
                *pPhyAddress = (int)hClientObj->hDriver->phyAddress;
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
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {

            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_NEG_RESTART, DRV_ETHPHY_RES_PENDING);
            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// PHY_CLIENT_OP_TYPE_NEG_RESTART
static void F_ETHPHY_ClientOpNegRestart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(DRV_PHY_NegRestartPhasesTbl)/sizeof(*DRV_PHY_NegRestartPhasesTbl))
    {
        (*DRV_PHY_NegRestartPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void F_ETHPHY_NegRestartPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the capabilities control reg: BMSTAT
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESTART_PHASE_READ, 0);
    }
}

static void F_ETHPHY_NegRestartPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMSTATbits_t  phyStdCpbl;

    // wait the BMSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyStdCpbl.w = hClientObj->smiData;
    if(phyStdCpbl.AN_ABLE == 0U)
    {   // no negotiation ability!
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_UNABLE);
        return;
    }

    // we can perform auto negotiation
    // restart negotiation and we'll have to wait
    if(F_DRV_PHY_SMIWriteStart(hClientObj, (uint16_t)PHY_REG_BMCON, (uint16_t)BMCON_AN_ENABLE_MASK | (uint16_t)BMCON_AN_RESTART_MASK ))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESTART_PHASE_WRITE, 0);
    }
}

static void F_ETHPHY_NegRestartPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}

DRV_ETHPHY_RESULT DRV_ETHPHY_HWConfigFlagsGet( DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags )
{
    //#todo# remove direct MAC register access from this function 
    DRV_ETHPHY_CONFIG_FLAGS hwFlags = DRV_ETHPHY_CFG_RMII;
    DRV_ETHPHY_RESULT ethRes = DRV_ETHPHY_RES_CFG_ERR;

    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
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
            hwFlags = ((GMAC_REGS->GMAC_UR & GMAC_UR_Msk)== (uint32_t)DRV_GMAC_RMII_MODE) ?  DRV_ETHPHY_CFG_RMII : DRV_ETHPHY_CFG_MII;
            ethRes = DRV_ETHPHY_RES_OK;
#elif defined (TCPIP_IF_PIC32CXBZ6_ETH)
            hwFlags = ((ETH_REGS->ETH_UR & ETH_UR_Msk)== (uint32_t)DRV_ETH_RMII_MODE) ?  DRV_ETHPHY_CFG_RMII : DRV_ETHPHY_CFG_MII;
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
            if((pGmacRegs->GMAC_NCR & GMAC_NCR_MIIONRGMII_Msk) != 0U)
            {
                if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == 0U) //GMAC_UR_MIM_MII
                {
                    hwFlags = DRV_ETHPHY_CFG_MII;
                    ethRes = DRV_ETHPHY_RES_OK;
                }
            }
            else
            {
                if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == (uint32_t)GMAC_UR_MIM_RGMII)
                {
                    hwFlags = DRV_ETHPHY_CFG_RGMII;
                    ethRes = DRV_ETHPHY_RES_OK;
                }
                else if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == (uint32_t)GMAC_UR_MIM_RMII)
                {
                    hwFlags = DRV_ETHPHY_CFG_RMII;
                    ethRes = DRV_ETHPHY_RES_OK;
                }
                else
                {
                    // do nothing
                }
            }  
#elif defined(TCPIP_IF_GMAC_SAM9X7) || defined(DRV_SAMA7D6)
            gmac_registers_t *  pGmacRegs = (gmac_registers_t *) hClientObj->ethphyId;
            if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == (uint32_t)GMAC_UR_MIM_RGMII)
            {
                hwFlags = DRV_ETHPHY_CFG_RGMII;
                ethRes = DRV_ETHPHY_RES_OK;
            }
            else if ((pGmacRegs->GMAC_UR & GMAC_UR_MIM_Msk) == (uint32_t)GMAC_UR_MIM_RMII)
            {
                hwFlags = DRV_ETHPHY_CFG_RMII;
                ethRes = DRV_ETHPHY_RES_OK;
            }            
            else
            {
                // do nothing
            }
#elif defined(TCPIP_IF_EMAC0) || defined(TCPIP_IF_EMAC1)
            hwFlags = DRV_ETHPHY_CFG_RMII;
            ethRes = DRV_ETHPHY_RES_OK;
#else
            hwFlags = DRV_ETHPHY_CFG_DEFAULT;
            ethRes = DRV_ETHPHY_RES_CFG_ERR;
#endif

            if(pFlags != NULL)
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
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            hClientObj->operParam = (uintptr_t)waitComplete;
            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_NEG_COMPLETE, DRV_ETHPHY_RES_PENDING);
            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// PHY_CLIENT_OP_TYPE_NEG_COMPLETE
static void F_ETHPHY_ClientOpNegComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(DRV_PHY_NegCompletePhasesTbl)/sizeof(*DRV_PHY_NegCompletePhasesTbl))
    {
        (*DRV_PHY_NegCompletePhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void F_ETHPHY_NegCompletePhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the control reg: PHY_REG_BMCON
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMCON))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_COMPLETE_PHASE_READ1, 0);
    }
}

static void F_ETHPHY_NegCompletePhaseRead1(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMCONbits_t   phyBMCon;
    // wait the BMCON read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyBMCon.w = hClientObj->smiData;
    if(phyBMCon.AN_ENABLE == 0U)
    {   // no negotiation is taking place; we're done
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_INACTIVE);
        return;
    }

    // re-issue another BMCON read
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMCON))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_COMPLETE_PHASE_READ2, 0);
    }
}

static void F_ETHPHY_NegCompletePhaseRead2(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
    // wait the BMCON read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save BMCON read for later
    hClientObj->operReg[1] = hClientObj->smiData;

    if(hClientObj->operParam == 0U)
    {   // no wait complete
        // issue a PHY_REG_BMSTAT read
        if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_COMPLETE_PHASE_RESULT_NOWAIT, 0);
        }
        return;
    }

    // wait for negotiation completion
    // and launch another BMCON read op
    uint32_t temp_tickFreq = SYS_TMR_TickCounterFrequencyGet();
    hClientObj->operTStamp = SYS_TMR_TickCountGet() + (uint32_t)((((phyInst->ethphyTmo->aNegInitTmo) * temp_tickFreq) + 999U) / 1000U);
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMCON))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_COMPLETE_PHASE_AN_RESTART, 0);
    }
}

static void F_ETHPHY_NegCompletePhaseAN_Restart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMCONbits_t   phyBMCon;
    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
    
    // wait the BMCON read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save BMCON read for later
    hClientObj->operReg[1] = phyBMCon.w = hClientObj->smiData;

    if(phyBMCon.AN_RESTART == 0U)
    {   // negotiation started; wait to complete
        // and launch another BMSTAT read op
        uint32_t temp_tickFreq = SYS_TMR_TickCounterFrequencyGet();
        hClientObj->operTStamp = SYS_TMR_TickCountGet() + (uint32_t)((((phyInst->ethphyTmo->aNegDoneTmo) * temp_tickFreq) + 999U) / 1000U);
        if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_COMPLETE_PHASE_AN_COMPLETE, 0);
        }
        return;
    }

    if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
    {   // wait some more; re-read
        (void)F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMCON);
    }
    else
    {   // timeout
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED);
    }
}

static void F_ETHPHY_NegCompletePhaseAN_Complete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMCONbits_t   phyBMCon;
    BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyStat.w = hClientObj->smiData;

    if(phyStat.AN_COMPLETE == 0U)
    {   // not done yet
        if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
        {   // wait some more
            (void)F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT);
        }
        else
        {   // timeout
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
        }
    }
    else
    {
        phyBMCon.w = hClientObj->operReg[1];   // restore BMCON read
        F_DRV_PHY_NegCompleteSetOperResult(hClientObj, phyBMCon.w, phyStat.w);
    }

}

static void F_ETHPHY_NegCompletePhaseResultNoWait(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMCONbits_t   phyBMCon;
    BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyStat.w = hClientObj->smiData;
    phyBMCon.w = hClientObj->operReg[1];   // restore BMCON read

    F_DRV_PHY_NegCompleteSetOperResult(hClientObj, phyBMCon.w, phyStat.w);
}

static void F_DRV_PHY_NegCompleteSetOperResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t phyBmConReg, uint16_t phyStatReg)
{
    DRV_ETHPHY_RESULT res;
    BMCONbits_t   phyBMCon;
    BMSTATbits_t  phyStat;

    phyBMCon.w = phyBmConReg;
    phyStat.w = phyStatReg;

    if(phyBMCon.AN_ENABLE == 0U)
    {
        res = DRV_ETHPHY_RES_NEGOTIATION_INACTIVE;       // no negotiation is taking place!
    }
    else if(phyBMCon.AN_RESTART != 0U)
    {
        res = DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED;        // not started yet/tmo
    }
    else
    {
        res = (phyStat.AN_COMPLETE == 0U) ? DRV_ETHPHY_RES_NEGOTIATION_ACTIVE : DRV_ETHPHY_RES_OK;    // active/tmo/ok
    }

    F_ETHPHY_SetOperDoneResult(hClientObj, res);
}



DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            // basic sanity check
            if(pNegResult == NULL)
            {
                return DRV_ETHPHY_RES_OPERATION_ERR;
            }

            hClientObj->operParam = (uintptr_t)pNegResult;
            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_NEG_RESULT, DRV_ETHPHY_RES_PENDING);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


// PHY_CLIENT_OP_TYPE_NEG_RESULT
static void F_ETHPHY_ClientOpNegResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if((unsigned int)hClientObj->operPhase < sizeof(DRV_PHY_NegResultPhasesTbl)/sizeof(*DRV_PHY_NegResultPhasesTbl))
    {
        (*DRV_PHY_NegResultPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void F_ETHPHY_NegResultPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the control reg: PHY_REG_BMSTAT
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_BMSTAT, 0);
    }
}

static void F_ETHPHY_NegResultPhase_BMSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMSTATbits_t   phyStat;
    // wait the BMSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    phyStat.w = hClientObj->smiData;
    if(phyStat.AN_COMPLETE == 0U)
    {   // negotiation is not yet done; we're finished
        F_DRV_PHY_NegResultSetOperResult(hClientObj, (DRV_ETHPHY_LINK_STATUS)((uint16_t)DRV_ETHPHY_LINK_ST_DOWN | (uint16_t)DRV_ETHPHY_LINK_ST_NEG_TMO), (TCPIP_ETH_OPEN_FLAGS)0, TCPIP_ETH_PAUSE_TYPE_NONE, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
    }
    else if(phyStat.LINK_STAT == 0U)
    {
        F_DRV_PHY_NegResultSetOperResult(hClientObj, DRV_ETHPHY_LINK_ST_DOWN, (TCPIP_ETH_OPEN_FLAGS)0, TCPIP_ETH_PAUSE_TYPE_NONE, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
    }
    else if(phyStat.EXTENDED_STAT == 1U)
    {
        // we're up and running
        // read the control reg: Extended Status
        if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_EXTSTAT))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_EXTSTAT, 0);
        }
    }
    else
    {   // we're up and running
        // read the control reg: ANEXP
        if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_ANEXP))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_ANEXP, 0);
        }
    }
}

static void F_ETHPHY_NegResultPhase_EXTSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the EXTSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    if (((hClientObj->smiData) & ((uint16_t)EXTSTAT_BASE1000X_FDX_MASK | (uint16_t)EXTSTAT_BASE1000X_HDX_MASK | (uint16_t)EXTSTAT_BASE1000T_FDX_MASK | (uint16_t)EXTSTAT_BASE1000T_HDX_MASK)) != 0U)
    {
        // read the control reg: 1000BASET-Control 
        if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_1000BASECON))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_1000BASECTRL, 0);
        }
    }
    else
    {
        // read the control reg: ANEXP
        if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_ANEXP))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_ANEXP, 0);
        }
    }
    
}    
static void F_ETHPHY_NegResultPhase_CTRL1000BASE(DRV_ETHPHY_CLIENT_OBJ * hClientObj)    
{    
    // wait the 1000BASET-Control read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save the 1000BASET-Control for later
    hClientObj->operReg[2] = hClientObj->smiData;
    // read the control reg: 1000BASESTAT
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_1000BASESTAT))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_1000BASESTAT, 0);
    }

}

static void F_ETHPHY_NegResultPhase_STAT1000BASE(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the 1000BASESTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save the 1000BASESTAT for later
    hClientObj->operReg[3] = hClientObj->smiData;

    // we're up and running
    // read the control reg: ANEXP
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_ANEXP))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_ANEXP, 0);
    }

}

static void F_ETHPHY_NegResultPhase_ANEXP(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the ANEXP read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save the ANEXP for later
    hClientObj->operReg[0] = hClientObj->smiData;

    // read the control reg: ANLPAD
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_ANLPAD))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_ANLPAD, 0);
    }

}

static void F_ETHPHY_NegResultPhase_ANLPAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the ANLPAD read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // save the ANLPAD for later
    hClientObj->operReg[1] = hClientObj->smiData;

    // read the control reg: ANAD
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_ANAD))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)PHY_NEG_RESULT_PHASE_ANAD, 0);
    }
}

static void F_ETHPHY_NegResultPhase_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    ANEXPbits_t    phyExp;
    ANLPADbits_t   lpAD;
    ANADbits_t     anadReg;
    uint16_t gigbCtrl = 0, gigbStat = 0;
    // pause capabilities, local and LP
    uint8_t lcl_Pause, lcl_AsmDir, lp_Pause, lp_AsmDir;     
    uint16_t  linkStat;
    uint32_t   oFlags;
    uint8_t   pauseType;

    // wait the ANAD read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
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
    pauseType = (uint8_t)TCPIP_ETH_PAUSE_TYPE_NONE;
    linkStat = (uint16_t)DRV_ETHPHY_LINK_ST_UP;

    lcl_Pause  = ((hClientObj->hDriver->macPauseType & (uint8_t)TCPIP_ETH_PAUSE_TYPE_PAUSE) != 0U) ? 1U : 0U;
    lcl_AsmDir = ((hClientObj->hDriver->macPauseType & (uint8_t)TCPIP_ETH_PAUSE_TYPE_ASM_DIR) != 0U) ? 1U : 0U;
    lp_Pause   = lp_AsmDir = 0;         // in case negotiation failed

    if(phyExp.LP_AN_ABLE != 0U)
    {   // ok,valid auto negotiation info

        if(lpAD.REM_FAULT != 0U)
        {
            linkStat |= (uint16_t)DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
        }

        if(lpAD.PAUSE != 0U)
        {
            linkStat |= (uint16_t)DRV_ETHPHY_LINK_ST_LP_PAUSE;
            lp_Pause = 1;
        }
        if(lpAD.ASM_DIR != 0U)
        {
            linkStat |= (uint16_t)DRV_ETHPHY_LINK_ST_LP_ASM_DIR;
            lp_AsmDir = 1;
        }
    }
    else
    {   // even if negotiation is not enabled, the link partner advertising shows the configuration
        linkStat |= (uint16_t)DRV_ETHPHY_LINK_ST_LP_NEG_UNABLE;
        if(phyExp.PDF != 0U)
        {
            linkStat |= (uint16_t)DRV_ETHPHY_LINK_ST_PDF;
        }
    }

    // set the PHY connection params
    // advertised capabilities & the matching ones
    anadReg.w &= lpAD.w;
    // get the settings, according to IEEE 802.3 Annex 28B.3 Priority Resolution
    // Note: we don't support 100BaseT4 !
    if(((gigbStat & (uint16_t)STAT1000B_1000T_FDX_MASK) != 0U) && ((gigbCtrl & (uint16_t)TCTRL_1000BASE_TFDX_MASK) != 0U))
    {
        oFlags = ((uint32_t)TCPIP_ETH_OPEN_1000 | (uint32_t)TCPIP_ETH_OPEN_FDUPLEX);
    }
    else if(((gigbStat & (uint16_t)STAT1000B_1000T_HDX_MASK)!= 0U) && ((gigbCtrl & (uint16_t)TCTRL_1000BASE_THDX_MASK)!= 0U))
    {
        oFlags = ((uint32_t)TCPIP_ETH_OPEN_1000 | (uint32_t)TCPIP_ETH_OPEN_HDUPLEX);
    }
    else if((anadReg.w & (uint16_t)ANAD_BASE100TX_FDX_MASK)!= 0U)
    {
        oFlags = ((uint32_t)TCPIP_ETH_OPEN_100 | (uint32_t)TCPIP_ETH_OPEN_FDUPLEX);
    }
    else if((anadReg.w & (uint16_t)ANAD_BASE100TX_MASK)!= 0U)
    {
        oFlags = ((uint32_t)TCPIP_ETH_OPEN_100 | (uint32_t)TCPIP_ETH_OPEN_HDUPLEX);
    }
    else if((anadReg.w & (uint16_t)ANAD_BASE10T_FDX_MASK)!= 0U)
    {
        oFlags = ((uint32_t)TCPIP_ETH_OPEN_10 | (uint32_t)TCPIP_ETH_OPEN_FDUPLEX);
    }
    else if((anadReg.w & (uint16_t)ANAD_BASE10T_MASK)!= 0U)
    {
        oFlags = ((uint32_t)TCPIP_ETH_OPEN_10 | (uint32_t)TCPIP_ETH_OPEN_HDUPLEX);
    }
    else
    {   // this should NOT happen!
        linkStat |= (uint16_t)DRV_ETHPHY_LINK_ST_NEG_FATAL_ERR;
        linkStat &= ~(uint16_t)DRV_ETHPHY_LINK_ST_UP;      // make sure we stop...!
    }

    // set the pause type for the MAC
    // according to IEEE Std 802.3-2002 Tables 28B-2, 28B-3
    if((oFlags & (uint32_t)TCPIP_ETH_OPEN_FDUPLEX) != 0U)
    {   // pause type relevant for full duplex only
        if((lp_Pause & (lcl_Pause | (lcl_AsmDir & lp_AsmDir))) != 0U)
        {
            pauseType = (uint8_t)TCPIP_ETH_PAUSE_TYPE_EN_TX;
        }
        if((lcl_Pause & (lp_Pause | (lcl_AsmDir & lp_AsmDir))) != 0U)
        {
            pauseType |= (uint8_t)TCPIP_ETH_PAUSE_TYPE_EN_RX;
        }
    }

    F_DRV_PHY_NegResultSetOperResult(hClientObj, (DRV_ETHPHY_LINK_STATUS)linkStat, (TCPIP_ETH_OPEN_FLAGS)oFlags, (TCPIP_ETH_PAUSE_TYPE)pauseType, DRV_ETHPHY_RES_OK);
}


static void F_DRV_PHY_NegResultSetOperResult(DRV_ETHPHY_CLIENT_OBJ* hClientObj, DRV_ETHPHY_LINK_STATUS linkStatus,
       TCPIP_ETH_OPEN_FLAGS openFlags, TCPIP_ETH_PAUSE_TYPE pauseType, DRV_ETHPHY_RESULT res)
{
    DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult = (DRV_ETHPHY_NEGOTIATION_RESULT*)hClientObj->operParam;

    if(pNegResult != NULL)
    {
        pNegResult->linkStatus = linkStatus;
        pNegResult->linkFlags = openFlags;
        pNegResult->pauseType = pauseType;
    }

    F_ETHPHY_SetOperDoneResult(hClientObj, res);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_INTERFACE_INDEX portIndex, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
{
    (void)portIndex;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            // basic sanity check
            if(pLinkStat == NULL)
            {
                return DRV_ETHPHY_RES_OPERATION_ERR;
            }

            hClientObj->operParam = (uintptr_t)pLinkStat;
            hClientObj->operReg[0] = (uint16_t)refresh;
            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_LINK_STAT, DRV_ETHPHY_RES_PENDING);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}
 
// PHY_CLIENT_OP_TYPE_LINK_STAT
static void F_ETHPHY_ClientOpLinkStat(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(DRV_PHY_LinkStatPhasesTbl)/sizeof(*DRV_PHY_LinkStatPhasesTbl))
    {
        (*DRV_PHY_LinkStatPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

// DRV_ETHPHY_LINK_STAT_PHASE_IDLE
static void F_ETHPHY_LinkStatPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the link status: PHY_REG_BMSTAT
    if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_LINK_STAT_PHASE_READ, 0);
    }
}

// read from PHY_REG_BMSTAT
// DRV_ETHPHY_LINK_STAT_PHASE_READ
static void F_ETHPHY_LinkStatPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // read the link status
    phyStat.w = hClientObj->smiData;
    if((phyStat.LINK_STAT != 0U) || (hClientObj->operReg[0] == 0U))
    {   // link up or no refresh needed
        DRV_ETHPHY_LINK_STATUS* pLinkStat = (DRV_ETHPHY_LINK_STATUS*)hClientObj->operParam;
        if(pLinkStat != (DRV_ETHPHY_LINK_STATUS*)0)
        {
            *pLinkStat = F_Phy2LinkStat(phyStat);
        }
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
    }
    else
    {   // initiate another read
        if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMSTAT))
        {
            F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_LINK_STAT_PHASE_REFRESH, 0);
        }
    }
}

// read from PHY_REG_BMSTAT
// DRV_ETHPHY_LINK_STAT_PHASE_REFRESH
static void F_ETHPHY_LinkStatPhaseRefresh(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
    {
        return;
    }

    // read the link status
    phyStat.w = hClientObj->smiData;

    DRV_ETHPHY_LINK_STATUS* pLinkStat = (DRV_ETHPHY_LINK_STATUS*)hClientObj->operParam;
    if(pLinkStat != NULL)
    {
        *pLinkStat = F_Phy2LinkStat(phyStat);
    }

    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_Reset( DRV_HANDLE handle, bool waitComplete )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {

            if(hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_READY)
            {   // another op going on
                return DRV_ETHPHY_RES_NOT_READY_ERR;
            }

            hClientObj->operParam = (uintptr_t)waitComplete;
            F_ETHPHY_SetOperStart(hClientObj, PHY_CLIENT_OP_TYPE_RESET, DRV_ETHPHY_RES_PENDING);

            return DRV_ETHPHY_RES_PENDING;
        }
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}


// PHY_CLIENT_OP_TYPE_RESET
static void F_ETHPHY_ClientOpReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(DRV_PHY_ResetPhasesTbl)/sizeof(*DRV_PHY_ResetPhasesTbl))
    {
        (*DRV_PHY_ResetPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void F_DRV_ETHPHY_ResetPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // initiate Soft Reset of the PHY
    if(F_DRV_PHY_SMIWriteStart(hClientObj, (uint16_t)PHY_REG_BMCON, BMCON_RESET_MASK))
    {
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_RESET_PHASE_WRITE, 0);
    }
}

static void F_DRV_ETHPHY_ResetPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
    if( hClientObj->operParam == 0U)
    {   // no need to wait, we're done
        F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
    }
    else
    {   // read the BMCON
        uint32_t temp_tickFreq = SYS_TMR_TickCounterFrequencyGet();
        hClientObj->operTStamp = SYS_TMR_TickCountGet() + (uint32_t)((((phyInst->ethphyTmo->resetTmo) * temp_tickFreq) + 999U) / 1000U);
        F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_RESET_PHASE_READ, 0);
    }
}

static void F_DRV_ETHPHY_ResetPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    BMCONbits_t bmcon;

    switch(hClientObj->operSubPhase)
    {
        case 0U:
            // initiate BMCON read
            if(F_DRV_PHY_SMIReadStart(hClientObj, (uint16_t)PHY_REG_BMCON))
            {
                F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_RESET_PHASE_READ, 1);
            }
            break;

        case 1U:
            // wait the BMCON read to complete
            if(!F_DRV_PHY_SMITransfer_Wait(hClientObj))
            {
                break;
            }

            bmcon.w = hClientObj->smiData;
            if(bmcon.RESET != 0U)
            {   // wait to clear itself
                if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
                {   // not yet tmo; re-read
                    F_DRV_PHY_SetOperPhase(hClientObj, (uint16_t)DRV_ETHPHY_RESET_PHASE_READ, 0);
                }
                else
                {   // failed by timeout
                    F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_RST_TMO_ERR);
                }
            }
            else
            {   // reset cleared itself; success
                F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
            }
            break;

        default:
            // shouldn't happen
            F_ETHPHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
            break;
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

    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
        {
            if(pVendorData != NULL)
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

    if(hClientObj != NULL)
    {
        DRV_ETHPHY_INSTANCE * phyInst = F_ETHPHY_HandleToInst(hClientObj->hDriver);
        if(phyInst != NULL)
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

    if((hClientObj == NULL) || (F_ETHPHY_HandleToInst(hClientObj->hDriver) == NULL))
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }
    
    if( (hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_BUSY) || 
        (hClientObj->operType != (uint16_t)PHY_CLIENT_OP_TYPE_SETUP) || 
        ((hClientObj->operPhase != (uint16_t)DRV_ETHPHY_SETUP_PHASE_DETECT) && (hClientObj->operPhase != (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE)))
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(hClientObj->smiTxferStatus != (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_NONE)
    {   // calling this before the previous SMI done?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }


    opRes = F_DRV_PHY_SMIReadStartEx(hClientObj, rIx, phyAddress);
    if(opRes < (DRV_ETHPHY_SMI_TXFER_RES)0)
    {   // error occurred 
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_BUSY)
    {
        hClientObj->smiTxferStatus = (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_NONE;
        return DRV_ETHPHY_RES_PENDING;
    } 
    else
    {
        //Do Nothing
    }

    // went through
    return DRV_ETHPHY_RES_OK;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadResultGet( DRV_HANDLE handle, uint16_t* pSmiRes)
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if((hClientObj == NULL) || (F_ETHPHY_HandleToInst(hClientObj->hDriver) == NULL))
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if( (hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_BUSY) || 
        (hClientObj->operType != (uint16_t)PHY_CLIENT_OP_TYPE_SETUP) || 
        ((hClientObj->operPhase != (uint16_t)DRV_ETHPHY_SETUP_PHASE_DETECT) && (hClientObj->operPhase != (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE)))
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(hClientObj->smiTxferStatus != (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_WAIT_COMPLETE)
    {   // not called as a result of a previous read?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    DRV_ETHPHY_SMI_TXFER_RES opRes = F_DRV_PHY_SMITransferWaitComplete(hClientObj);
    if(opRes < (DRV_ETHPHY_SMI_TXFER_RES)0)
    {   // error
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_WAIT)
    {   // wait some more
        return DRV_ETHPHY_RES_PENDING;
    }
    else
    {
        //Do Nothing
    }
    // done

    if(pSmiRes != NULL)
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

    if((hClientObj == NULL) || (F_ETHPHY_HandleToInst(hClientObj->hDriver) == NULL))
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    
    if( (hClientObj->status != (int16_t)DRV_ETHPHY_CLIENT_STATUS_BUSY) || 
        (hClientObj->operType != (uint16_t)PHY_CLIENT_OP_TYPE_SETUP) || 
        ((hClientObj->operPhase != (uint16_t)DRV_ETHPHY_SETUP_PHASE_DETECT) && (hClientObj->operPhase != (uint16_t)DRV_ETHPHY_SETUP_PHASE_NEGOTIATE)))
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(hClientObj->smiTxferStatus != (uint8_t)DRV_ETHPHY_SMI_TXFER_OP_NONE)
    {   // calling this before the previous SMI done?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    opRes = F_DRV_PHY_SMIWriteStartEx(hClientObj, rIx, wData, phyAddress, waitComplete);

    if(opRes < (DRV_ETHPHY_SMI_TXFER_RES)0)
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
    
    DRV_ETHPHY_SMI_TXFER_RES opRes = F_DRV_PHY_SMITransferWaitComplete(hClientObj);
    if(opRes < (DRV_ETHPHY_SMI_TXFER_RES)0)
    {   // error
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }
    else if(opRes == DRV_ETHPHY_SMI_TXFER_RES_WAIT)
    {   // wait some more
        return DRV_ETHPHY_RES_PENDING;
    }
    else
    {
        //Do Nothing
    }
    return DRV_ETHPHY_RES_OK;
}
/*******************************************************************************
End of File
*/
